#pragma once

#include <Urchin3dEngine.h>
#include <UrchinAIEngine.h>

class NPCNavigationEventHandler : public urchin::AICharacterEventHandler {
    public:
        explicit NPCNavigationEventHandler(urchin::Model *);

        void startMoving() override;
        void stopMoving() override;

    private:
        urchin::Model *model;
};
