#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <map>

#include <ScreenSwitcher.h>
#include <WindowController.h>
#include <util/CrashReporter.h>
#include <MainContext.h>

int main(int, char *[]);

class Main {
    public:
        Main();
        void execute(int, char *[]);

    private:
        std::unique_ptr<MainContext> createMainContext(GLFWwindow*, bool) const;
        static void glfwErrorCallback(int, const char*);
        void initializeInputKeyMap();

        static std::string retrieveResourcesDirectory(char *[]);

        static GLFWwindow *createWindow(bool);

        static void charCallback(GLFWwindow*, unsigned int);
        static void keyCallback(GLFWwindow*, int, int, int, int);
        static void mouseKeyCallback(GLFWwindow*, int, int, int);
        static void cursorPositionCallback(GLFWwindow*, double, double);
        static void scrollCallback(GLFWwindow*, double, double);
        static void framebufferSizeCallback(GLFWwindow*, int, int);

        void handleInputEvents();

        void onChar(char32_t);
        void onKeyPressed(int);
        void onKeyReleased(int);
        void onMouseButtonPressed(int);
        void onMouseButtonReleased(int);
        void onMouseMove(double, double) const;
        void onScroll(double) const;
        urchin::Control::Key toInputKey(int);

        bool argumentsContains(const std::string&, int, char *[]) const;

        void clearResources(GLFWwindow*&);

        std::shared_ptr<CrashReporter> crashReporter;
        std::unique_ptr<MainContext> context;

        bool propagatePressKeyEvent;
        bool propagateReleaseKeyEvent;
        std::map<int, urchin::Control::Key> inputKeyMap;
        std::vector<unsigned int> charEvents;
        std::vector<std::pair<int, bool>> keyEvents;
};
