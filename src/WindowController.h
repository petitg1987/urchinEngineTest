#ifndef URCHINENGINETEST_WINDOWCONTROLLER_H
#define URCHINENGINETEST_WINDOWCONTROLLER_H

class GLFWwindow;

class WindowController
{
    public:
        explicit WindowController(GLFWwindow *);

        void moveMouse(unsigned int, unsigned int) const;
        void setMouseCursorVisible(bool);

        void cleanEvents();
        bool isEventCallbackActive() const;

        void setVerticalSyncEnabled(bool);

    private:
        GLFWwindow *window;
        bool eventsCallbackActive;

};

#endif
