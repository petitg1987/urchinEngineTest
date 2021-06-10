#include <RenderScreen.h>
#include <MainDisplayer.h>
using namespace urchin;

RenderScreen::RenderScreen(MainDisplayer* mainDisplayer) :
        mainDisplayer(mainDisplayer) {

}

void RenderScreen::onKeyPressed(Control::Key) {

}

void RenderScreen::onKeyReleased(Control::Key) {

}

void RenderScreen::refresh() {

}

MainDisplayer* RenderScreen::getMainDisplayer() const {
    return mainDisplayer;
}
