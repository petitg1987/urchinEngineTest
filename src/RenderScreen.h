#pragma once

#include "UrchinCommon.h"

class MainDisplayer;

class RenderScreen {
    public:
        explicit RenderScreen(MainDisplayer *);
        virtual ~RenderScreen() = default;

        virtual void onKeyPressed(urchin::Control::Key);
        virtual void onKeyReleased(urchin::Control::Key);

        virtual void active(bool) = 0;
        virtual bool isActive() const = 0;

        virtual void refresh();

    protected:
        MainDisplayer *getMainDisplayer() const;

    private:
        MainDisplayer* mainDisplayer;
};
