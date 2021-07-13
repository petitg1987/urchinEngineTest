#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cassert>

#include <WindowController.h>

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
    int intWidthInPixel, intHeightInPixel;
    glfwGetFramebufferSize(window, &intWidthInPixel, &intHeightInPixel); //don't use glfwGetWindowSize which doesn't return size in pixel

    //window is probably minimized: wait for a valid width/height size
    while (intWidthInPixel == 0 || intHeightInPixel == 0) {
        glfwGetFramebufferSize(window, &intWidthInPixel, &intHeightInPixel);
        glfwWaitEvents();
    }

    widthInPixel = (unsigned int)intWidthInPixel;
    heightInPixel = (unsigned int)intHeightInPixel;
}

WindowController::WindowController(GLFWwindow* window, bool isDebugModeOn) :
        window(window),
        debugModeOn(isDebugModeOn),
        eventsCallbackActive(true) {

}

void WindowController::setMouseCursorVisible(bool visible) {
    int cursorMode = GLFW_CURSOR_NORMAL;
    if (!visible) {
        if (isDebugModeOn()) {
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

bool WindowController::isDebugModeOn() const {
    //Manual mouse adjustment (ensure mouse is not going outside the window) is required in debug mode.
    //Indeed, the GLFW native mode to control camera (GLFW_CURSOR_DISABLED) does not display the cursor when a breakpoint is caught.
    //Therefore, in debug mode, we hide the cursor (GLFW_CURSOR_HIDDEN) which automatically reappears when a breakpoint is caught.
    //However, this debug method can not be used in production because it is not behaves the same way on different computers.
    return debugModeOn;
}

void WindowController::moveMouse(double mouseX, double mouseY) const {
    assert(isDebugModeOn());
    glfwSetCursorPos(window, mouseX, mouseY);
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

