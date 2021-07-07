#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

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

WindowController::WindowController(GLFWwindow* window, bool isDebugAttached) :
        window(window),
        isDebugAttached(isDebugAttached),
        eventsCallbackActive(true) {

}

void WindowController::setMouseCursorVisible(bool visible) {
    int cursorMode = visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
    if (isDebugAttached) {
        cursorMode = GLFW_CURSOR_NORMAL;
    }

    glfwSetInputMode(window, GLFW_CURSOR, cursorMode);
    if (!visible && glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
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

