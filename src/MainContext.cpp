#include <MainContext.h>

MainContext::MainContext(std::unique_ptr<urchin::Scene> scene, std::unique_ptr<WindowController> windowController, std::unique_ptr<urchin::SoundEnvironment> soundEnvironment,
                         std::unique_ptr<ScreenHandler> screenSwitcher) :
        scene(std::move(scene)),
        windowController(std::move(windowController)),
        soundEnvironment(std::move(soundEnvironment)),
        screenSwitcher(std::move(screenSwitcher)) {
    getScreenSwitcher().initialize(*this);
}

WindowController& MainContext::getWindowController() const {
    return *windowController;
}

urchin::Scene& MainContext::getScene() const {
    return *scene;
}

urchin::SoundEnvironment& MainContext::getSoundEnvironment() const {
    return *soundEnvironment;
}

ScreenHandler& MainContext::getScreenSwitcher() const {
    return *screenSwitcher;
}
