#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <WindowController.h>

class CharacterCamera : public urchin::Camera {
    public:
        CharacterCamera(WindowController*, float, float, float);
        ~CharacterCamera() override = default;

        void updateCameraView(float) override;

        bool onMouseMove(double, double) override;

    private:
        const WindowController* windowController;
};
