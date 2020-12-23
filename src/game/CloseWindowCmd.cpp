#include "CloseWindowCmd.h"
using namespace urchin;

CloseWindowCmd::CloseWindowCmd(Window* window):
        window(window) {

}

void CloseWindowCmd::onClickRelease(Widget*) {
    window->setIsVisible(false);
}
