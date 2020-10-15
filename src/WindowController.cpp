#include <SFML/Window.hpp>

#include "WindowController.h"

WindowController::WindowController(sf::Window *window) :
 window(window)
{

}

void WindowController::moveMouse(unsigned int x, unsigned int y) const
{
    sf::Mouse::setPosition(sf::Vector2i(x, y), *window);
}

void WindowController::setMouseCursorVisible(bool visible)
{
    window->setMouseCursorVisible(visible);
}

void WindowController::cleanEvents()
{
    sf::Event event{};
    while (window->pollEvent(event))
        ;
}

void WindowController::setVerticalSyncEnabled(bool active)
{
    window->setVerticalSyncEnabled(active);
}
