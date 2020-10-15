#ifndef URCHINENGINETEST_MAIN_H
#define URCHINENGINETEST_MAIN_H

#include <list>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "MainDisplayer.h"

int main(int, char *[]);

class Main
{
    public:
        Main();
        void execute(int, char *[]);

    private:
        MainDisplayer *mainDisplayer;
        bool propagatePressKeyEvent, propagateReleaseKeyEvent;

        std::map<sf::Keyboard::Key, InputDevice::Key> keyboardMap;

        void initializeKeyboardMap();

        std::string retrieveResourcesDirectory(char *[]) const;
        std::string retrieveSaveDirectory(char *[]) const;

        std::list<sf::Event> retrieveOrderedEvents(sf::Window *) const;

        void onChar(const sf::Event &);
        void onKeyPressed(const sf::Event &);
        void onKeyReleased(const sf::Event &);

        void onMouseButtonPressed(const sf::Event &);
        void onMouseButtonReleased(const sf::Event &);
        void onMouseMove(const sf::Event &);

        bool argumentsContains(const std::string &, int, char *[]) const;

        void clearResources(sf::Window *&, WindowController *&);
        void failureExit(sf::Window *&, WindowController *&);
};

#endif
