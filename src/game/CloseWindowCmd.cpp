#include <game/CloseWindowCmd.h>
using namespace urchin;

CloseWindowCmd::CloseWindowCmd(Window* window):
        window(window) {

}

void CloseWindowCmd::onMouseLeftClickRelease(Widget*) {
    window->setIsVisible(false);
}
