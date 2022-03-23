#pragma once

#include <UrchinAIEngine.h>

class NPCNavigationEventHandler : public urchin::AICharacterEventHandler {
    public:
        void startMoving() override;
        void stopMoving() override;
};
