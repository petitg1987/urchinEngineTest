#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <list>
#include <map>

#include <MainDisplayer.h>
#include <WindowController.h>
#include <util/CrashReporter.h>

int main(int, char *[]);

class Main {
    public:
        Main();
        void execute(int, char *[]);

    private:
        static void glfwErrorCallback(int, const char*);
        void initializeInputKeyMap();

        static std::string retrieveResourcesDirectory(char *[]);
        static std::string retrieveSaveDirectory(char *[]);

        static GLFWwindow *createWindow(int, char *[]);

        static void charCallback(GLFWwindow*, unsigned int);
        static void keyCallback(GLFWwindow*, int, int, int, int);
        static void mouseKeyCallback(GLFWwindow*, int, int, int);
        static void cursorPositionCallback(GLFWwindow*, double, double);
        static void framebufferSizeCallback(GLFWwindow*, int, int);

        void handleInputEvents();

        void onChar(char32_t);
        void onKeyPressed(int);
        void onKeyReleased(int);
        void onMouseButtonPressed(int);
        void onMouseButtonReleased(int);
        void onMouseMove(int, int);
        urchin::Control::Key toInputKey(int);

        static bool argumentsContains(const std::string&, int, char *[]);

        void clearResources(GLFWwindow *&, WindowController *&);

        std::shared_ptr<CrashReporter> crashReporter;
        static std::string glfwError;

        MainDisplayer* mainDisplayer;
        WindowController* windowController;

        bool propagatePressKeyEvent, propagateReleaseKeyEvent;
        std::map<int, urchin::Control::Key> inputKeyMap;
        std::list<char32_t> charPressEvents;
        std::list<int> keyPressEvents;
};
