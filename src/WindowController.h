#pragma once

#include <vector>
#include <memory>

#include <graphics/setup/SurfaceCreator.h>
#include <graphics/setup/FramebufferSizeRetriever.h>

class SDL_Window;

struct SdlSurfaceCreator final : public urchin::SurfaceCreator {
    explicit SdlSurfaceCreator(SDL_Window*);
    void* createSurface(void*) override;

    SDL_Window* window;
};

struct SdlFramebufferSizeRetriever final : public urchin::FramebufferSizeRetriever {
    explicit SdlFramebufferSizeRetriever(SDL_Window*);
    void getFramebufferSizeInPixel(unsigned int&, unsigned int&) const override;

    SDL_Window* window;
};

class WindowController {
    public:
        WindowController(SDL_Window*, bool);

        static urchin::Point2<int> optimumWindowSize();

        void updateWindowedMode(bool);

        bool isDevModeOn() const;
        void setMouseCursorVisible(bool);
        bool isMouseCursorVisible() const;
        void moveMouse(double, double) const;
        urchin::Point2<double> getMousePosition() const;

        void cleanEvents();
        bool isEventCallbackActive() const;

        std::vector<std::string> windowRequiredExtensions();
        std::unique_ptr<SdlSurfaceCreator> newSurfaceCreator() const;
        std::unique_ptr<SdlFramebufferSizeRetriever> newFramebufferSizeRetriever() const;

    private:
        SDL_Window* window;
        bool devModeOn;
        bool eventsCallbackActive;
        urchin::Point2<double> lastVisibleMouseCoord;
};
