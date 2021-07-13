#pragma once

#include <vector>
#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>

#include <RenderScreen.h>
#include <game/CharacterCamera.h>
#include <game/UnderWaterEvent.h>
#include <game/npc/NPCNavigation.h>

class MainDisplayer;

class GameRenderer : public RenderScreen {
    public:
        explicit GameRenderer(MainDisplayer *);
        ~GameRenderer() override;

        void onKeyPressed(urchin::Control::Key) override;
        void onKeyReleased(urchin::Control::Key) override;

        void active(bool) override;
        bool isActive() const override;

        void refresh() override;

    private:
        void initialize();

        void initializeCharacter();
        void uninitializeCharacter();

        void initializeWaterEvent();
        void uninitializeWaterEvent();

        void initializeNPC();
        void uninitializeNPC();

        void switchMode();
        void deleteGeometryModels(std::vector<urchin::GeometryModel *>&) const;

        urchin::SunLight *getSunLight();

        urchin::Vector3<float> getWalkVelocity() const;
        urchin::RigidBody *getRandomInactiveBody();

        bool isInitialized;
        bool editMode;
        bool memCheckMode;

        //3d
        urchin::Renderer3d* gameRenderer3d;
        std::shared_ptr<CharacterCamera> camera;
        UnderWaterEvent* underWaterEvent;

        //physics
        std::unique_ptr<urchin::PhysicsWorld> physicsWorld;
        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        urchin::CharacterControllerConfig characterControllerConfig;
        std::unique_ptr<urchin::CharacterController> characterController;
        bool leftKeyPressed, rightKeyPressed, upKeyPressed, downKeyPressed;
        std::vector<urchin::GeometryModel*> rayModels;
        std::unique_ptr<urchin::CollisionPointDisplayer> collisionPointsDisplayer;

        //AI
        std::unique_ptr<urchin::AIManager> aiManager;
        std::vector<urchin::GeometryModel*> pathModels;
        std::unique_ptr<urchin::NavMeshDisplayer> navMeshDisplayer;

        //map handler
        std::unique_ptr<urchin::MapHandler> mapHandler;

        //UI
        urchin::UIRenderer* gameUIRenderer;
        urchin::Text* fpsText;

        //sound
        urchin::ManualTrigger* manualTrigger;

        //NPC navigation
        std::unique_ptr<NPCNavigation> npcNavigation;
};
