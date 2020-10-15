#ifndef URCHINENGINETEST_WINDOWCONTROLLER_H
#define URCHINENGINETEST_WINDOWCONTROLLER_H

namespace sf {
    class Window;
}

class WindowController
{
    public:
        explicit WindowController(sf::Window *);

        void moveMouse(unsigned int, unsigned int) const;
        void setMouseCursorVisible(bool);

        void cleanEvents();

        void setVerticalSyncEnabled(bool);

    private:
        sf::Window *window;

};

#endif
