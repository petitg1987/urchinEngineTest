#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <game/GameRenderer.h>

class MainDisplayer {
    public:
        explicit MainDisplayer(WindowController *);
        ~MainDisplayer();

        void initialize(const std::string&);
        void paint();
        void resize();
        void onKeyPressed(urchin::Control::Key);
        void onKeyReleased(urchin::Control::Key);
        void onMouseMove(int, int);

        int getMouseX() const;
        int getMouseY() const;

        WindowController *getWindowController() const;
        urchin::SceneManager *getSceneManager() const;
        urchin::SoundManager *getSoundManager() const;

    private:
        WindowController* windowController;
        int mouseX, mouseY;

        urchin::SceneManager *sceneManager;
        urchin::SoundManager *soundManager;

        RenderScreen* gameRenderer;
};
