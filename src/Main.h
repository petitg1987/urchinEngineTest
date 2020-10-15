#ifndef URCHINENGINETEST_MAIN_H
#define URCHINENGINETEST_MAIN_H

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <list>

#include "MainDisplayer.h"

int main(int, char *[]);

class Main
{
    public:
        Main();
        void execute(int, char *[]);

    private:
        static void glfwErrorCallback(int, const char*);
        void initializeKeyboardMap();

        std::string retrieveResourcesDirectory(char *[]) const;
        std::string retrieveSaveDirectory(char *[]) const;

        static void charCallback(GLFWwindow *, unsigned int, int);
        static void keyCallback(GLFWwindow *, int, int, int, int);
        static void mouseKeyCallback(GLFWwindow *, int, int, int);
        static void cursorPositionCallback(GLFWwindow *, double, double);
        static void windowSizeCallback(GLFWwindow *, int, int);

//        std::list<sf::Event> retrieveOrderedEvents(sf::Window *) const;

        void onChar(unsigned int);
        void onKeyPressed(int);
        void onKeyReleased(int);

        void onMouseButtonPressed(int);
        void onMouseButtonReleased(int);
        void onMouseMove(int, int);

        bool argumentsContains(const std::string &, int, char *[]) const;

        void clearResources(GLFWwindow *&, WindowController *&);
        void failureExit(GLFWwindow *&, WindowController *&);

        MainDisplayer *mainDisplayer;
        bool propagatePressKeyEvent, propagateReleaseKeyEvent;

        std::map<int, InputDevice::Key> keyboardMap;
};

#endif
