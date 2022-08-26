#pragma once

#include <Urchin3dEngine.h>

class CloseWindowCmd final : public urchin::EventListener {
    public:
        explicit CloseWindowCmd(urchin::Window&);

        bool onMouseLeftClickRelease(urchin::Widget*) override;

    private:
        urchin::Window& window;
};
