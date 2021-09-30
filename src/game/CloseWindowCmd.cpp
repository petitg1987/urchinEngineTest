#include <game/CloseWindowCmd.h>
using namespace urchin;

CloseWindowCmd::CloseWindowCmd(Window& window):
        window(window) {

}

bool CloseWindowCmd::onMouseLeftClickRelease(Widget*) {
    window.setIsVisible(false);
    return false;
}
