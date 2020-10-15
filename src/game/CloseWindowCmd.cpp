#include "CloseWindowCmd.h"

CloseWindowCmd::CloseWindowCmd(urchin::Window *window):
    window(window)
{

}

void CloseWindowCmd::onClickRelease(urchin::Widget *)
{
    window->setIsVisible(false);
}
