#include <memory>

#include <ScreenHandler.h>
#include <MainContext.h>
using namespace urchin;

ScreenHandler::ScreenHandler() :
        context(nullptr),
        currentScreen(nullptr) {

}

void ScreenHandler::initialize(MainContext& context) {
    this->context = &context;

    gameRenderer = std::make_unique<GameRenderer>(context);

    switchToScreen(*gameRenderer);
}

void ScreenHandler::paint() {
    currentScreen->refresh();
    context->getScene().display();
}

void ScreenHandler::resize() {
    context->getScene().onResize();
}

void ScreenHandler::onKeyPressed(Control::Key key) {
    currentScreen->onKeyPressed(key);
}

void ScreenHandler::onKeyReleased(Control::Key key) {
    currentScreen->onKeyReleased(key);
}

void ScreenHandler::onMouseMove(double x, double y) {
    currentScreen->onMouseMove(x, y);
}

void ScreenHandler::switchToScreen(Screen& screen) {
    if (this->currentScreen != &screen) {
        if (this->currentScreen) {
            this->currentScreen->enable(false);
        }
        this->currentScreen = &screen;
        screen.enable(true);
    }
}
