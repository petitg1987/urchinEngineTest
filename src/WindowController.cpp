#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "WindowController.h"

WindowController::WindowController(GLFWwindow *window) :
 window(window)
{

}

void WindowController::moveMouse(unsigned int x, unsigned int y) const
{
    glfwSetCursorPos(window, x, y);
}

void WindowController::setMouseCursorVisible(bool visible)
{
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);

}

void WindowController::setVerticalSyncEnabled(bool active)
{
    glfwSwapInterval( active ? 1 : 0);
}
