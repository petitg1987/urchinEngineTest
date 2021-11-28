#include <AbstractScreen.h>
using namespace urchin;

AbstractScreen::AbstractScreen(MainContext& context) :
        context(context) {

}

void AbstractScreen::onKeyPressed(Control::Key) {
    //can be overridden
}

void AbstractScreen::onKeyReleased(Control::Key) {
    //can be overridden
}

void AbstractScreen::onMouseMove(double, double) {
    //can be overridden
}

void AbstractScreen::refresh() {
    //can be overridden
}

MainContext& AbstractScreen::getContext() const {
    return context;
}
