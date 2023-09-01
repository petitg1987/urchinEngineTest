#include <game/CharacterCamera.h>
using namespace urchin;

CharacterCamera::CharacterCamera(const WindowController& windowController, float horizontalFovAngle, float nearPlane, float farPlane) :
        Camera(horizontalFovAngle, nearPlane, farPlane),
        windowController(windowController) {
    useMouseToMoveCamera(true);
}

bool CharacterCamera::onMouseMove(double mouseX, double mouseY) {
    bool propagateEvent = Camera::onMouseMove(mouseX, mouseY);

    if (windowController.isDevModeOn() && isUseMouseToMoveCamera()) {
        windowController.moveMouse((double)getSceneWidth() / 2.0, (double)getSceneHeight() / 2.0);
    }
    return propagateEvent;
}
