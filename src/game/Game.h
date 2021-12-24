#pragma once

#include <vector>
#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinAggregation.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>

#include <game/CharacterCamera.h>
#include <game/UnderWaterEvent.h>
#include <game/npc/NPCNavigation.h>

class MainContext;

class Game {
    public:
        explicit Game(MainContext&);
        ~Game();

        void onKeyPressed(urchin::Control::Key);
        void onKeyReleased(urchin::Control::Key);

        void refresh();

    private:
        void initialize();

        void initializeCharacter();
        void uninitializeCharacter();

        void initializeWaterEvent();
        void uninitializeWaterEvent();

        void initializeNPC();
        void uninitializeNPC();

        void switchMode();
        void deleteGeometryModels(std::vector<std::shared_ptr<urchin::GeometryModel>>&) const;

        urchin::SunLight *getSunLight();

        urchin::Vector3<float> computeCharacterVelocity() const;
        urchin::RigidBody *getRandomInactiveBody();

        MainContext& context;
        bool editMode;

        //3d
        urchin::Renderer3d* gameRenderer3d;
        std::shared_ptr<CharacterCamera> camera;
        std::unique_ptr<UnderWaterEvent> underWaterEvent;

        //physics
        std::unique_ptr<urchin::PhysicsWorld> physicsWorld;
        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        urchin::CharacterControllerConfig characterControllerConfig;
        std::unique_ptr<urchin::CharacterController> characterController;
        bool leftKeyPressed, rightKeyPressed, upKeyPressed, downKeyPressed;
        std::vector<std::shared_ptr<urchin::GeometryModel>> rayModels;
        std::unique_ptr<urchin::CollisionPointDisplayer> collisionPointsDisplayer;

        //AI
        std::unique_ptr<urchin::AIEnvironment> aiEnvironment;
        std::vector<std::shared_ptr<urchin::GeometryModel>> pathModels;
        std::unique_ptr<urchin::NavMeshDisplayer> navMeshDisplayer;

        //map
        std::unique_ptr<urchin::Map> map;

        //UI
        urchin::UIRenderer* gameUIRenderer;
        std::shared_ptr<urchin::Text> fpsText;

        //sound
        urchin::ManualTrigger* manualTrigger;

        //NPC navigation
        std::unique_ptr<NPCNavigation> npcNavigation;
};
