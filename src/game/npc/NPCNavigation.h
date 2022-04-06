#pragma once

#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinAIEngine.h>

class NPCNavigation {
    public:
        NPCNavigation(float, float, const urchin::Map&, urchin::AIEnvironment&, urchin::PhysicsWorld&);

        std::shared_ptr<const urchin::PathRequest> getPathRequest() const;

        void display(const urchin::Scene& scene);

    private:
        urchin::Model *model;

        std::unique_ptr<urchin::CharacterController> characterController;
        std::unique_ptr<urchin::AICharacterController> aiCharacterController;
};
