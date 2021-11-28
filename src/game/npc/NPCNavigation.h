#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

class ScreenHandler;

class NPCNavigation {
    public:
        NPCNavigation(float, float, const urchin::MapHandler&, urchin::AIEnvironment&, urchin::PhysicsWorld&);

        std::shared_ptr<const urchin::PathRequest> getPathRequest() const;

        void display(const urchin::Scene& scene);

    private:
        urchin::Model *model;

        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        std::unique_ptr<urchin::CharacterController> characterController;
        std::shared_ptr<urchin::AICharacter> aiCharacter;
        std::unique_ptr<urchin::AICharacterController> aiCharacterController;
};
