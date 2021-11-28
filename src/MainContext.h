#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <ScreenHandler.h>

class MainContext {
    public:
        MainContext(std::unique_ptr<urchin::Scene>, std::unique_ptr<WindowController>, std::unique_ptr<urchin::SoundEnvironment>, std::unique_ptr<ScreenHandler>);

        urchin::Scene& getScene() const;
        WindowController& getWindowController() const;
        urchin::SoundEnvironment& getSoundEnvironment() const;
        ScreenHandler& getScreenSwitcher() const;

    private:
        std::unique_ptr<urchin::Scene> scene;
        std::unique_ptr<WindowController> windowController;
        std::unique_ptr<urchin::SoundEnvironment> soundEnvironment;
        std::unique_ptr<ScreenHandler> screenSwitcher;
};
