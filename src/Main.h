#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

#include <WindowController.h>
#include <util/CrashReporter.h>
#include <MainContext.h>
#include <game/Game.h>

class SDL_Window;

int main(int, char *[]);

class Main {
    public:
        Main();
        void execute(std::span<char*>);

    private:
        std::unique_ptr<MainContext> createMainContext(SDL_Window*, bool) const;
        void initializeInputKeyMap();

        static std::string retrieveResourcesDirectory(const char*);

        static SDL_Window* createWindow(bool);

        void handleInputEvents(SDL_Window*);

        void onChar(char32_t);
        void onKeyPressed(int, bool);
        void onKeyReleased(int);
        void onMouseButtonPressed(int);
        void onMouseButtonReleased(int);
        void onMouseMove(double, double, double, double) const;
        void onScroll(double) const;
        urchin::Control::Key toInputKey(int);

        bool argumentsContains(const std::string&, std::span<char*>) const;

        void clearResources(SDL_Window*&);

        std::shared_ptr<CrashReporter> crashReporter;
        std::unique_ptr<MainContext> context;
        std::unique_ptr<Game> game;

        bool propagatePressKeyEvent;
        bool propagateReleaseKeyEvent;
        std::map<int, urchin::Control::Key> inputKeyMap;
        std::vector<SDL_Event> events;
        bool altLeftKeyPressed;
};
