#ifndef URCHINENGINETEST_CHARACTERCAMERA_H
#define URCHINENGINETEST_CHARACTERCAMERA_H

#include "UrchinCommon.h"
#include "Urchin3dEngine.h"

#include "WindowController.h"

class CharacterCamera : public urchin::Camera {
    public:
        CharacterCamera(float, float, float, const WindowController*);
        ~CharacterCamera() override = default;

        void moveMouse(unsigned int, unsigned int) override;

        void updateCameraView(float) override;

    private:
        const WindowController *windowController;
};

#endif
