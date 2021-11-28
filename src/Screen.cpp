#include <Screen.h>
using namespace urchin;

Screen::Screen(MainContext& context) :
        context(context) {

}

void Screen::onKeyPressed(Control::Key) {
    //can be overridden
}

void Screen::onKeyReleased(Control::Key) {
    //can be overridden
}

void Screen::onMouseMove(double, double) {
    //can be overridden
}

void Screen::refresh() {
    //can be overridden
}

MainContext& Screen::getContext() const {
    return context;
}
