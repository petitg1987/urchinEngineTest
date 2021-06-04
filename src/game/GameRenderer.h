#pragma once

#include <vector>
#include <memory>
#include <UrchinCommon.h>
#include <Urchin3dEngine.h>
#include <UrchinMapHandler.h>
#include <UrchinPhysicsEngine.h>
#include <UrchinSoundEngine.h>
#include <UrchinAIEngine.h>

#include <KeyboardKey.h>
#include <RenderScreen.h>
#include <game/CharacterCamera.h>
#include <game/UnderWaterEvent.h>
#include <game/npc/NPCNavigation.h>

class MainDisplayer;

class GameRenderer : public RenderScreen {
    public:
        explicit GameRenderer(MainDisplayer *);
        ~GameRenderer() override;

        void onKeyPressed(KeyboardKey) override;
        void onKeyReleased(KeyboardKey) override;

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

        urchin::Vector3<float> getWalkMomentum() const;
        urchin::RigidBody *getRandomUnactiveBody();

        bool isInitialized;
        bool editMode;
        bool memCheckMode;

        //3d
        urchin::Renderer3d *gameRenderer3d;
        urchin::MapHandler *mapHandler;
        CharacterCamera* camera;
        UnderWaterEvent* underWaterEvent;

        //physics
        urchin::PhysicsWorld *physicsWorld;
        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        std::unique_ptr<urchin::PhysicsCharacterController> physicsCharacterController;
        bool leftKeyPressed, rightKeyPressed, upKeyPressed, downKeyPressed;
        std::vector<urchin::GeometryModel *> rayModels;
        std::unique_ptr<urchin::CollisionPointDisplayer> collisionPointsDisplayer;

        //AI
        urchin::AIManager *aiManager;
        std::vector<urchin::GeometryModel *> pathModels;
        std::unique_ptr<urchin::NavMeshDisplayer> navMeshDisplayer;

        //UI
        urchin::UIRenderer *gameUIRenderer;
        urchin::Text *fpsText;

        urchin::Text *myText;
        urchin::Button *myButton;
        urchin::TextBox *textBox;
        urchin::Window*myWindow, *myWindow2;
        urchin::Slider *mySlider;

        //sound
        urchin::ManualTrigger *manualTrigger;

        //NPC navigation
        std::unique_ptr<NPCNavigation> npcNavigation;
};
