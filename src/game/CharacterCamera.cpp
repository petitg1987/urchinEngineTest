#include <game/CharacterCamera.h>
using namespace urchin;

CharacterCamera::CharacterCamera(const WindowController& windowController, float horizontalFovAngle, float nearPlane, float farPlane) :
        Camera(horizontalFovAngle, nearPlane, farPlane),
        windowController(windowController) {
    useMouseToMoveCamera(true);
}

void CharacterCamera::refreshCameraView(float) {
    //nothing to do
}

bool CharacterCamera::onMouseMove(double mouseX, double mouseY) {
    bool propagateEvent = Camera::onMouseMove(mouseX, mouseY);

    if (windowController.isDebugModeOn() && isUseMouseToMoveCamera()) {
        windowController.moveMouse((double)getSceneWidth() / 2.0, (double)getSceneHeight() / 2.0);

        Point2<double> mousePosition = windowController.getMousePosition(); //mouse position previously set could differ from the one retrieved due to values with decimal
        resetPreviousMousePosition(mousePosition.X, mousePosition.Y);
    }
    return propagateEvent;
}
