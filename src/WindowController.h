#pragma once

#include <vector>
#include <memory>

#include <graphics/setup/SurfaceCreator.h>
#include <graphics/setup/FramebufferSizeRetriever.h>

class GLFWwindow;
class GLFWmonitor;

struct ExtensionList {
    uint32_t extensionCount = 0;
    const char** extensions = nullptr;
};

struct GlfwSurfaceCreator final : public urchin::SurfaceCreator {
    explicit GlfwSurfaceCreator(GLFWwindow*);
    void* createSurface(void*) override;

    GLFWwindow* window;
};

struct GlfwFramebufferSizeRetriever final : public urchin::FramebufferSizeRetriever {
    explicit GlfwFramebufferSizeRetriever(GLFWwindow*);
    void getFramebufferSizeInPixel(unsigned int&, unsigned int&) const override;

    GLFWwindow* window;
};

class WindowController {
    public:
        WindowController(GLFWwindow *, bool);

        static urchin::Point2<int> optimumWindowSize();

        void updateWindowedMode(bool);

        void setMouseCursorVisible(bool);
        bool isMouseCursorVisible() const;
        bool isDevModeOn() const;
        void moveMouse(double, double) const;
        urchin::Point2<double> getMousePosition() const;

        void cleanEvents();
        bool isEventCallbackActive() const;

        static std::vector<std::string> windowRequiredExtensions();
        std::unique_ptr<GlfwSurfaceCreator> newSurfaceCreator() const;
        std::unique_ptr<GlfwFramebufferSizeRetriever> newFramebufferSizeRetriever() const;

    private:
        GLFWmonitor* getWindowMonitor() const;

        GLFWwindow* window;
        bool devModeOn;
        bool eventsCallbackActive;

};
