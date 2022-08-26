#pragma once

#include <UrchinAIEngine.h>

class NPCNavigationEventHandler final : public urchin::AICharacterEventHandler {
    public:
        void startMoving() override;
        void stopMoving() override;
};
