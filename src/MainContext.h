#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <ScreenSwitcher.h>

class MainContext {
    public:
        MainContext(std::unique_ptr<urchin::Scene>, std::unique_ptr<WindowController>, std::unique_ptr<urchin::SoundEnvironment>, std::unique_ptr<ScreenSwitcher>);

        urchin::Scene& getScene() const;
        WindowController& getWindowController() const;
        urchin::SoundEnvironment& getSoundEnvironment() const;
        ScreenSwitcher& getScreenSwitcher() const;

    private:
        std::unique_ptr<urchin::Scene> scene;
        std::unique_ptr<WindowController> windowController;
        std::unique_ptr<urchin::SoundEnvironment> soundEnvironment;
        std::unique_ptr<ScreenSwitcher> screenSwitcher;
};
