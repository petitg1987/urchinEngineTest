#ifndef URCHINENGINETEST_WINDOWCONTROLLER_H
#define URCHINENGINETEST_WINDOWCONTROLLER_H

#include <vector>
#include <memory>

#include "graphic/setup/spi/SurfaceCreator.h"
#include "graphic/setup/spi/FramebufferSizeRetriever.h"

class GLFWwindow;

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
    void getFramebufferSizeInPixel(int&, int&) const override;

    GLFWwindow *window;
};

class WindowController {
    public:
        explicit WindowController(GLFWwindow *);

        void moveMouse(unsigned int, unsigned int) const;
        void setMouseCursorVisible(bool);

        void cleanEvents();
        bool isEventCallbackActive() const;

        static std::vector<std::string> windowRequiredExtensions();
        std::unique_ptr<GlfwSurfaceCreator> getSurfaceCreator() const;
        std::unique_ptr<GlfwFramebufferSizeRetriever> getFramebufferSizeRetriever() const;

    private:
        GLFWwindow* window;
        bool eventsCallbackActive;

};

#endif
