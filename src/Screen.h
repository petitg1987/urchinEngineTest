#pragma once

#include "UrchinCommon.h"

class ScreenHandler;

class Screen {
    public:
        explicit Screen(ScreenHandler *);
        virtual ~Screen() = default;

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
