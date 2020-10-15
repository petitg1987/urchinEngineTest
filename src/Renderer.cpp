#include "Renderer.h"
#include "MainDisplayer.h"

Renderer::Renderer(MainDisplayer *mainDisplayer) :
    mainDisplayer(mainDisplayer)
{

}

void Renderer::onKeyPressed(InputDevice::Key)
{

}

void Renderer::onKeyReleased(InputDevice::Key)
{

}

void Renderer::refresh()
{

}

MainDisplayer *Renderer::getMainDisplayer() const
{
    return mainDisplayer;
}
