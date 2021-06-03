#include <game/CharacterCamera.h>
using namespace urchin;

CharacterCamera::CharacterCamera(float angle, float nearPlane, float farPlane, const WindowController* windowController) :
        Camera(angle, nearPlane, farPlane),
        windowController(windowController) {
    useMouseToMoveCamera(true);
}

void CharacterCamera::moveMouse(unsigned int x, unsigned int y) {
    windowController->moveMouse(x, y);
}

void CharacterCamera::updateCameraView(float) {
    //nothing to do
}
