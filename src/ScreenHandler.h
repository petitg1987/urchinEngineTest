#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinSoundEngine.h>

#include <WindowController.h>
#include <game/GameRenderer.h>

class MainContext;

class ScreenHandler {
    public:
        ScreenHandler();

        void initialize(MainContext&);

        void paint();
        void resize();
        void onKeyPressed(urchin::Control::Key);
        void onKeyReleased(urchin::Control::Key);
        void onMouseMove(double, double);

        void switchToScreen(Screen&);

    private:
        MainContext* context;

        std::unique_ptr<Screen> gameRenderer;

        Screen* currentScreen;
};
