#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <WindowController.h>

class CharacterCamera : public urchin::Camera {
    public:
        CharacterCamera(float, float, float);
        ~CharacterCamera() override = default;

        void updateCameraView(float) override;

    private:
        const WindowController* windowController;
};
