#include <memory>

#include <ScreenSwitcher.h>
#include <MainContext.h>
using namespace urchin;

ScreenSwitcher::ScreenSwitcher() :
        currentScreen(nullptr) {

}

void ScreenSwitcher::initialize(MainContext& context) {
    game = std::make_unique<Game>(context);

    switchToScreen(*game);
}

void ScreenSwitcher::refresh() {
    currentScreen->refresh();
}

void ScreenSwitcher::onKeyPressed(Control::Key key) {
    currentScreen->onKeyPressed(key);
}

void ScreenSwitcher::onKeyReleased(Control::Key key) {
    currentScreen->onKeyReleased(key);
}

void ScreenSwitcher::onMouseMove(double x, double y) {
    currentScreen->onMouseMove(x, y);
}

void ScreenSwitcher::switchToScreen(AbstractScreen& screen) {
    if (this->currentScreen != &screen) {
        if (this->currentScreen) {
            this->currentScreen->enable(false);
        }
        this->currentScreen = &screen;
        screen.enable(true);
    }
}
