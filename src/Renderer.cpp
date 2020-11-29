#include "Renderer.h"
#include "MainDisplayer.h"

Renderer::Renderer(MainDisplayer* mainDisplayer) :
    mainDisplayer(mainDisplayer) {

}

void Renderer::onKeyPressed(KeyboardKey) {

}

void Renderer::onKeyReleased(KeyboardKey) {

}

void Renderer::refresh() {

}

MainDisplayer* Renderer::getMainDisplayer() const {
    return mainDisplayer;
}
