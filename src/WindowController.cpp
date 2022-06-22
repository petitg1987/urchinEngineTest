#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cassert>

#include <WindowController.h>
using namespace urchin;

GlfwSurfaceCreator::GlfwSurfaceCreator(GLFWwindow* window) :
        window(window) {
}

VkSurfaceKHR GlfwSurfaceCreator::createSurface(VkInstance instance) {
    VkSurfaceKHR surface = nullptr;
    VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface with error code: " + std::to_string(result));
    }
    return surface;
}

GlfwFramebufferSizeRetriever::GlfwFramebufferSizeRetriever(GLFWwindow* window) :
        window(window) {
}

void GlfwFramebufferSizeRetriever::getFramebufferSizeInPixel(unsigned int& widthInPixel, unsigned int& heightInPixel) const {
    int intWidthInPixel;
    int intHeightInPixel;
    glfwGetFramebufferSize(window, &intWidthInPixel, &intHeightInPixel); //don't use glfwGetWindowSize which doesn't return size in pixel

    //window is probably minimized: wait for a valid width/height size
    while (intWidthInPixel == 0 || intHeightInPixel == 0) {
        glfwGetFramebufferSize(window, &intWidthInPixel, &intHeightInPixel);
        glfwWaitEvents();
    }

    widthInPixel = (unsigned int)intWidthInPixel;
    heightInPixel = (unsigned int)intHeightInPixel;
}

WindowController::WindowController(GLFWwindow* window, bool devModeOn) :
        window(window),
        devModeOn(devModeOn),
        eventsCallbackActive(true) {

}

Point2<int> WindowController::optimumWindowSize() {
    return Point2<int>(2560, 1440);
    //return Point2<int>(3440, 1440); //for wide screen test
}

void WindowController::updateWindowedMode(bool windowedModeEnabled) {
    bool isFullScreen = glfwGetWindowMonitor(window) != nullptr;
    if (isFullScreen && windowedModeEnabled) {
        Point2<int> optimumWindowSize = WindowController::optimumWindowSize();
        glfwSetWindowMonitor(window, nullptr, 100, 100 /* do not put 0 because the title bar will be outside the screen on Windows */, optimumWindowSize.X, optimumWindowSize.Y, GLFW_DONT_CARE);
        Logger::instance().logInfo("Switched to windowed mode");
    } else if (!isFullScreen && !windowedModeEnabled) {
        GLFWmonitor* monitor = getWindowMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        Logger::instance().logInfo("Switched to fullscreen mode");
    }
}

void WindowController::setMouseCursorVisible(bool visible) {
    int cursorMode = GLFW_CURSOR_NORMAL;
    if (!visible) {
        if (isDevModeOn()) {
            cursorMode = GLFW_CURSOR_HIDDEN;
        } else {
            cursorMode = GLFW_CURSOR_DISABLED;
        }
    }
    glfwSetInputMode(window, GLFW_CURSOR, cursorMode);

    if (glfwRawMouseMotionSupported()) {
        if (!visible) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        } else {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
        }
    }
}

bool WindowController::isMouseCursorVisible() const {
    return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL;
}

bool WindowController::isDevModeOn() const {
    return devModeOn;
}

void WindowController::moveMouse(double mouseX, double mouseY) const {
    #ifdef APP_DEBUG
        assert(isDevModeOn() || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
    #endif
    glfwSetCursorPos(window, mouseX, mouseY);
}

Point2<double> WindowController::getMousePosition() const {
    #ifdef APP_DEBUG
        assert(isDevModeOn() || glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
    #endif
    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    return Point2<double>(mouseX, mouseY);
}

void WindowController::cleanEvents() {
    eventsCallbackActive = false;
    glfwPollEvents();
    eventsCallbackActive = true;
}

bool WindowController::isEventCallbackActive() const {
    return eventsCallbackActive;
}

std::vector<std::string> WindowController::windowRequiredExtensions() {
    ExtensionList extensionList;
    extensionList.extensions = glfwGetRequiredInstanceExtensions(&extensionList.extensionCount);
    return std::vector<std::string>(extensionList.extensions, extensionList.extensions + extensionList.extensionCount);
}

std::unique_ptr<GlfwSurfaceCreator> WindowController::newSurfaceCreator() const {
    return std::make_unique<GlfwSurfaceCreator>(window);
}

std::unique_ptr<GlfwFramebufferSizeRetriever> WindowController::newFramebufferSizeRetriever() const {
    return std::make_unique<GlfwFramebufferSizeRetriever>(window);
}

/**
 * Return the current monitor where is the window.
 * Note: glfwGetWindowMonitor(), can not be used for two reasons:
 *      1. Method is not working correctly: https://github.com/glfw/glfw/issues/2137
 *      2. Method returns nullptr if window is not in fullscreen mode
 */
GLFWmonitor* WindowController::getWindowMonitor() const {
    int bestOverlap = 0;
    GLFWmonitor *bestMonitor = glfwGetPrimaryMonitor();

    int windowPosX = 0;
    int windowPosY = 0;
    glfwGetWindowPos(window, &windowPosX, &windowPosY);
    int windowSizeX = 0;
    int windowSizeY = 0;
    glfwGetWindowSize(window, &windowSizeX, &windowSizeY);

    int numMonitor = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&numMonitor);

    for (int monitorIndex = 0; monitorIndex < numMonitor; ++monitorIndex) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitors[monitorIndex]);
        int modeWidth = mode->width;
        int modeHeight = mode->height;

        int monitorX = 0;
        int monitorY = 0;
        glfwGetMonitorPos(monitors[monitorIndex], &monitorX, &monitorY);

        int overlap =
                std::max(0, std::min(windowPosX + windowSizeX, monitorX + modeWidth) - std::max(windowPosX, monitorX)) *
                std::max(0, std::min(windowPosY + windowSizeY, monitorY + modeHeight) - std::max(windowPosY, monitorY));

        if (overlap > bestOverlap) {
            bestOverlap = overlap;
            bestMonitor = monitors[monitorIndex];
        }
    }

    return bestMonitor;
}
