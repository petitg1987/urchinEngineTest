#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

class MainDisplayer;

class NPCNavigation {
    public:
        NPCNavigation(float, float, urchin::MapHandler*, urchin::AIManager*, urchin::PhysicsWorld&);

        std::shared_ptr<const urchin::PathRequest> getPathRequest() const;

        void display(MainDisplayer *);

    private:
        urchin::Model *model;

        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        std::unique_ptr<urchin::CharacterController> characterController;
        std::shared_ptr<urchin::AICharacter> aiCharacter;
        std::unique_ptr<urchin::AICharacterController> aiCharacterController;
};
