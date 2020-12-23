#ifndef URCHINENGINETEST_RENDERSCREEN_H
#define URCHINENGINETEST_RENDERSCREEN_H

#include "KeyboardKey.h"

class MainDisplayer;

class RenderScreen {
    public:
        explicit RenderScreen(MainDisplayer *);
        virtual ~RenderScreen() = default;

        virtual void onKeyPressed(KeyboardKey);
        virtual void onKeyReleased(KeyboardKey);

        virtual void active(bool) = 0;
        virtual bool isActive() const = 0;

        virtual void refresh();

    protected:
        MainDisplayer *getMainDisplayer() const;

    private:
        MainDisplayer* mainDisplayer;
};

#endif
