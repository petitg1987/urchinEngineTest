#pragma once

#include <Urchin3dEngine.h>

class CloseWindowCmd : public urchin::EventListener {
    public:
        explicit CloseWindowCmd(urchin::Window *);

        void onMouseLeftClickRelease(urchin::Widget *) override;

    private:
        urchin::Window *window;
};
