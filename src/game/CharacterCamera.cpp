#include <game/CharacterCamera.h>
using namespace urchin;

CharacterCamera::CharacterCamera(float angle, float nearPlane, float farPlane) :
        Camera(angle, nearPlane, farPlane) {
    useMouseToMoveCamera(true);
}

void CharacterCamera::updateCameraView(float) {
    //nothing to do
}
