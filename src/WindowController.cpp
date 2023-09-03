#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdexcept>

#include <WindowController.h>
using namespace urchin;

SdlSurfaceCreator::SdlSurfaceCreator(SDL_Window* window) :
        window(window) {
}

void* SdlSurfaceCreator::createSurface(void* instance) {
    VkSurfaceKHR surface = nullptr;
    SDL_bool result = SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance), &surface);
    if (result != SDL_TRUE) {
        throw std::runtime_error("Failed to create window surface with error code: " + std::string(SDL_GetError()));
    }
    return surface;
}

SdlFramebufferSizeRetriever::SdlFramebufferSizeRetriever(SDL_Window* window) :
        window(window) {
}

void SdlFramebufferSizeRetriever::getFramebufferSizeInPixel(unsigned int& widthInPixel, unsigned int& heightInPixel) const {
    widthInPixel = 0;
    heightInPixel = 0;

    //When the window is minimized or Alt+Tabbed: "SDL_Vulkan_GetDrawableSize" return a valid size (2560x1440) but "VkSurfaceCapabilitiesKHR#currentExtent" contains a zero size (0x0)
    //Therefore, we return an invalid size here to avoid creating a swap chain with a wrong image extend
    bool isMinimized = (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED;
    if (!isMinimized) {
        int widthInPixelInt = 0;
        int heightInPixelInt = 0;
        SDL_Vulkan_GetDrawableSize(window, &widthInPixelInt, &heightInPixelInt); //don't use SDL_GetWindowSize which doesn't return size in pixel
        widthInPixel = widthInPixelInt > 1 ? (unsigned int)widthInPixelInt : 0;
        heightInPixel = heightInPixelInt > 1 ? (unsigned int)heightInPixelInt : 0;
    }
}

WindowController::WindowController(SDL_Window* window, bool devModeOn) :
        window(window),
        devModeOn(devModeOn),
        eventsCallbackActive(true),
        lastVisibleMouseCoord(Point2<double>(0.0, 0.0)) {

}

Point2<int> WindowController::optimumWindowSize() {
    return Point2<int>(2560, 1440);
    //return Point2<int>(3440, 1440); //for wide screen test
}

void WindowController::updateWindowedMode(bool windowedModeEnabled) {
    bool isFullScreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
    if (isFullScreen && windowedModeEnabled) {
        if (SDL_SetWindowFullscreen(window, 0) != 0) {
            Logger::instance().logError("Switch to windowed mode cause error: " + std::string(SDL_GetError()));
        } else {
            Point2<int> windowSize = optimumWindowSize();
            SDL_SetWindowResizable(window, SDL_TRUE);
            SDL_SetWindowSize(window, windowSize.X, windowSize.Y);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            Logger::instance().logInfo("Switched to windowed mode");
        }
    } else if (!isFullScreen && !windowedModeEnabled) {
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) != 0) {
            Logger::instance().logError("Switch to fullscreen mode cause error: " + std::string(SDL_GetError()));
        } else {
            Logger::instance().logInfo("Switched to fullscreen mode");
        }
        SDL_SetWindowResizable(window, SDL_FALSE);
    }
}

bool WindowController::isDevModeOn() const {
    return devModeOn;
}

void WindowController::setMouseCursorVisible(bool visible) {
    bool resetMousePosition = false;
    int showCursor = SDL_ENABLE;
    SDL_bool relativeMouseMove = SDL_FALSE;

    if (visible) {
        resetMousePosition = !isMouseCursorVisible() && (lastVisibleMouseCoord.X != 0.0 || lastVisibleMouseCoord.Y != 0.0);
    } else {
        if (isDevModeOn()) {
            showCursor = SDL_DISABLE;
        } else {
            relativeMouseMove = SDL_TRUE;
        }

        if (isMouseCursorVisible()) {
            lastVisibleMouseCoord = getMousePosition();
        }
    }

    SDL_ShowCursor(showCursor);
    if (SDL_SetRelativeMouseMode(relativeMouseMove) != 0) {
        Logger::instance().logError("Impossible to update relative mouse mode state: " + std::string(SDL_GetError()));
    }
    if (resetMousePosition) {
        moveMouse(lastVisibleMouseCoord.X, lastVisibleMouseCoord.Y);
    }
}

bool WindowController::isMouseCursorVisible() const {
    return SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE && SDL_GetRelativeMouseMode() == SDL_FALSE;
}

void WindowController::moveMouse(double mouseX, double mouseY) const {
    SDL_WarpMouseInWindow(window, MathFunction::roundToInt(mouseX), MathFunction::roundToInt(mouseY));
}

Point2<double> WindowController::getMousePosition() const {
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    return Point2<double>(mouseX, mouseY);
}

void WindowController::cleanEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        //cleaning events
    }
    eventsCallbackActive = true;
}

bool WindowController::isEventCallbackActive() const {
    return eventsCallbackActive;
}

std::vector<std::string> WindowController::windowRequiredExtensions() {
    unsigned int count = 0;
    SDL_Vulkan_GetInstanceExtensions(window, &count, nullptr);

    const auto** pNames = new const char*[count];
    SDL_Vulkan_GetInstanceExtensions(window, &count, pNames);
    auto extensions = std::span(pNames, size_t(count));

    std::vector<std::string> extensionNames;
    extensionNames.reserve(extensions.size());
    for (const auto& extension : extensions) {
        extensionNames.emplace_back(extension);
    }
    delete[] pNames;

    return extensionNames;
}

std::unique_ptr<SdlSurfaceCreator> WindowController::newSurfaceCreator() const {
    return std::make_unique<SdlSurfaceCreator>(window);
}

std::unique_ptr<SdlFramebufferSizeRetriever> WindowController::newFramebufferSizeRetriever() const {
    return std::make_unique<SdlFramebufferSizeRetriever>(window);
}
