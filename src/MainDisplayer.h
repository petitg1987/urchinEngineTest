#ifndef URCHINENGINETEST_MAINDISPLAYER_H
#define URCHINENGINETEST_MAINDISPLAYER_H

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinSoundEngine.h"

#include "WindowController.h"
#include "KeyboardKey.h"
#include "game/GameRenderer.h"

class MainDisplayer {
    public:
        explicit MainDisplayer(WindowController *);
        ~MainDisplayer();

        void initialize(const std::string &, const std::string &);
        void paint();
        void resize(unsigned int width, unsigned int height);
        void onKeyPressed(KeyboardKey);
        void onKeyReleased(KeyboardKey);
        void onMouseMove(int, int);

        int getMouseX() const;
        int getMouseY() const;

        WindowController *getWindowController() const;
        urchin::SceneManager *getSceneManager() const;
        urchin::SoundManager *getSoundManager() const;

    private:
        WindowController *windowController;
        int mouseX, mouseY;

        urchin::SceneManager *sceneManager;
        urchin::SoundManager *soundManager;

        Renderer *gameRenderer;
};

#endif
