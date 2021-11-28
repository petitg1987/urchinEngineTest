#pragma once

#include "UrchinCommon.h"

class MainContext;

class AbstractScreen {
    public:
        explicit AbstractScreen(MainContext&);
        virtual ~AbstractScreen() = default;

        virtual void onKeyPressed(urchin::Control::Key);
        virtual void onKeyReleased(urchin::Control::Key);
        virtual void onMouseMove(double, double);

        virtual void enable(bool) = 0;
        virtual bool isEnabled() const = 0;

        virtual void refresh();

    protected:
        MainContext& getContext() const;

    private:
        MainContext& context;
};
