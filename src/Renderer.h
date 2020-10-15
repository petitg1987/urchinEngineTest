#ifndef URCHINENGINETEST_RENDERER_H
#define URCHINENGINETEST_RENDERER_H

#include "InputDevice.h"

class MainDisplayer;

class Renderer
{
    public:
        explicit Renderer(MainDisplayer *);
        virtual ~Renderer() = default;

        virtual void onKeyPressed(InputDevice::Key key);
        virtual void onKeyReleased(InputDevice::Key key);

        virtual void active(bool) = 0;
        virtual bool isActive() const = 0;

        virtual void refresh();

    protected:
        MainDisplayer *getMainDisplayer() const;

    private:
        MainDisplayer *mainDisplayer;
};

#endif
