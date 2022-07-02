#pragma once

#include <vector>
#include <memory>

#include <graphics/api/vulkan/setup/spi/SurfaceCreator.h>
#include <graphics/api/vulkan/setup/spi/FramebufferSizeRetriever.h>

class GLFWwindow;
class GLFWmonitor;

struct ExtensionList {
    uint32_t extensionCount = 0;
    const char** extensions = nullptr;
};

struct GlfwSurfaceCreator : public urchin::SurfaceCreator {
    explicit GlfwSurfaceCreator(GLFWwindow*);
    VkSurfaceKHR createSurface(VkInstance instance) override;

    GLFWwindow *window;
};

struct GlfwFramebufferSizeRetriever : public urchin::FramebufferSizeRetriever {
    explicit GlfwFramebufferSizeRetriever(GLFWwindow*);
    void getFramebufferSizeInPixel(unsigned int&, unsigned int&) const override;

    GLFWwindow *window;
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
