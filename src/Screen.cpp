#include <Screen.h>
#include <ScreenHandler.h>
using namespace urchin;

Screen::Screen(ScreenHandler* screenHandler) :
        screenHandler(screenHandler) {

}

void Screen::onKeyPressed(Control::Key) {
    //can be overridden
}

void Screen::onKeyReleased(Control::Key) {
    //can be overridden
}

void Screen::refresh() {
    //can be overridden
}

ScreenHandler* Screen::getScreenHandler() const {
    return screenHandler;
}
