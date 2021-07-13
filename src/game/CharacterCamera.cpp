#include <game/CharacterCamera.h>
using namespace urchin;

CharacterCamera::CharacterCamera(const WindowController& windowController, float angle, float nearPlane, float farPlane) :
        Camera(angle, nearPlane, farPlane),
        windowController(windowController) {
    useMouseToMoveCamera(true);
}

void CharacterCamera::updateCameraView(float) {
    //nothing to do
}

bool CharacterCamera::onMouseMove(double mouseX, double mouseY) {
    bool propagateEvent = Camera::onMouseMove(mouseX, mouseY);

    if (windowController.isDebugModeOn() && isUseMouseToMoveCamera()) {
        double halfWidth = (double)getSceneWidth() / 2.0;
        double halfHeight = (double)getSceneHeight() / 2.0;
        windowController.moveMouse(halfWidth, halfHeight);
        resetPreviousMousePosition(halfWidth, halfHeight);
    }
    return propagateEvent;
}
