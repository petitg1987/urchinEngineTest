#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <map>

#include <WindowController.h>
#include <util/CrashReporter.h>
#include <MainContext.h>
#include <game/Game.h>

int main(int, char *[]);

class Main {
    public:
        Main();
        void execute(std::span<char*>);

    private:
        std::unique_ptr<MainContext> createMainContext(GLFWwindow*, bool) const;
        static void glfwErrorCallback(int, const char*);
        void initializeInputKeyMap();

        static std::string retrieveResourcesDirectory(const char*);

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

        bool argumentsContains(const std::string&, std::span<char*>) const;

        void clearResources(GLFWwindow*&);

        std::shared_ptr<CrashReporter> crashReporter;
        std::unique_ptr<MainContext> context;
        std::unique_ptr<Game> game;

        bool propagatePressKeyEvent;
        bool propagateReleaseKeyEvent;
        std::map<int, urchin::Control::Key> inputKeyMap;
        std::vector<unsigned int> charEvents;
        struct KeyEvent {
            int key;
            bool isKeyPressed;
            bool isRepeatPress;
        };
        std::vector<KeyEvent> keyEvents;
};
