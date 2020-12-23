#include "RenderScreen.h"
#include "MainDisplayer.h"

RenderScreen::RenderScreen(MainDisplayer* mainDisplayer) :
    mainDisplayer(mainDisplayer) {

}

void RenderScreen::onKeyPressed(KeyboardKey) {

}

void RenderScreen::onKeyReleased(KeyboardKey) {

}

void RenderScreen::refresh() {

}

MainDisplayer* RenderScreen::getMainDisplayer() const {
    return mainDisplayer;
}
