#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "WindowController.h"

WindowController::WindowController(GLFWwindow* window) :
    window(window),
    eventsCallbackActive(true) {

}

void WindowController::moveMouse(unsigned int x, unsigned int y) const {
    glfwSetCursorPos(window, x, y);
}

void WindowController::setMouseCursorVisible(bool visible) {
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void WindowController::cleanEvents() {
    eventsCallbackActive = false;
    glfwPollEvents();
    eventsCallbackActive = true;
}

bool WindowController::isEventCallbackActive() const {
    return eventsCallbackActive;
}

void WindowController::setVerticalSyncEnabled(bool active) {
    glfwSwapInterval( active ? 1 : 0);
}

void WindowController::swapBuffers() {
    glfwSwapBuffers(window);
}
