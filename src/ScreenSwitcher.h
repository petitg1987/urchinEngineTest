#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <game/Game.h>

class MainContext;

class ScreenSwitcher {
    public:
        ScreenSwitcher();

        void initialize(MainContext&);
        void refresh();

        void onKeyPressed(urchin::Control::Key);
        void onKeyReleased(urchin::Control::Key);
        void onMouseMove(double, double);

        void switchToScreen(AbstractScreen&);

    private:
        std::unique_ptr<AbstractScreen> game;

        AbstractScreen* currentScreen;
};
