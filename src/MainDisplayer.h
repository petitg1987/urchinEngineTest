#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <game/GameRenderer.h>

class MainDisplayer {
    public:
        explicit MainDisplayer(WindowController&);
        ~MainDisplayer();

        void initialize(const std::string&);
        void paint();
        void resize();
        void onKeyPressed(urchin::Control::Key);
        void onKeyReleased(urchin::Control::Key);
        void onMouseMove(double, double);

        double getMouseX() const;
        double getMouseY() const;

        WindowController& getWindowController() const;
        urchin::Scene* getScene() const;
        urchin::SoundManager* getSoundManager() const;

    private:
        WindowController& windowController;
        double mouseX, mouseY;

        std::unique_ptr<urchin::Scene> scene;
        std::unique_ptr<urchin::SoundManager> soundManager;

        std::unique_ptr<RenderScreen> gameRenderer;
};
