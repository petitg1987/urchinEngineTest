#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>

#include <WindowController.h>

class CharacterCamera final : public urchin::Camera {
    public:
        CharacterCamera(const WindowController&, float, float, float);
        ~CharacterCamera() override = default;

        bool onMouseMove(double, double) override;

    private:
        const WindowController& windowController;
};
