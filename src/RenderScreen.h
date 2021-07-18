#pragma once

#include "UrchinCommon.h"

class ScreenHandler;

class RenderScreen {
    public:
        explicit RenderScreen(ScreenHandler *);
        virtual ~RenderScreen() = default;

        virtual void onKeyPressed(urchin::Control::Key);
        virtual void onKeyReleased(urchin::Control::Key);

        virtual void active(bool) = 0;
        virtual bool isActive() const = 0;

        virtual void refresh();

    protected:
        ScreenHandler *getScreenHandler() const;

    private:
        ScreenHandler* screenHandler;
};
