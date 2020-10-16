#ifndef URCHINENGINETEST_CLOSEWINDOWSCMD_H
#define URCHINENGINETEST_CLOSEWINDOWSCMD_H

#include "Urchin3dEngine.h"

class CloseWindowCmd : public urchin::EventListener {
    public:
        explicit CloseWindowCmd(urchin::Window *);

        void onClickRelease(urchin::Widget *) override;

    private:
        urchin::Window *window;
};

#endif
