#include <RenderScreen.h>
#include <ScreenHandler.h>
using namespace urchin;

RenderScreen::RenderScreen(ScreenHandler* screenHandler) :
        screenHandler(screenHandler) {

}

void RenderScreen::onKeyPressed(Control::Key) {

}

void RenderScreen::onKeyReleased(Control::Key) {

}

void RenderScreen::refresh() {

}

ScreenHandler* RenderScreen::getScreenHandler() const {
    return screenHandler;
}
