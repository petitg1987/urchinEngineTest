#include <Screen.h>
#include <ScreenHandler.h>
using namespace urchin;

Screen::Screen(ScreenHandler* screenHandler) :
        screenHandler(screenHandler) {

}

void Screen::onKeyPressed(Control::Key) {

}

void Screen::onKeyReleased(Control::Key) {

}

void Screen::refresh() {

}

ScreenHandler* Screen::getScreenHandler() const {
    return screenHandler;
}
