#ifndef URCHINENGINETEST_GAMERENDERER_H
#define URCHINENGINETEST_GAMERENDERER_H

#include <vector>
#include <memory>
#include "UrchinCommon.h"
#include "Urchin3dEngine.h"
#include "UrchinMapHandler.h"
#include "UrchinPhysicsEngine.h"
#include "UrchinSoundEngine.h"
#include "UrchinAIEngine.h"

#include "KeyboardKey.h"
#include "Renderer.h"
#include "game/CharacterCamera.h"

class MainDisplayer;

class GameRenderer : public Renderer
{
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

        void switchMode();
        void deleteGeometryModels(std::vector<urchin::GeometryModel *> &) const;

        urchin::SunLight *getSunLight();

        urchin::Vector3<float> getWalkMomentum() const;
        urchin::RigidBody *getRandomUnactiveBody();

        bool isInitialized;
        bool editMode;
        bool memCheckMode;

        //3d
        urchin::Renderer3d *gameRenderer3d;
        urchin::MapHandler *mapHandler;
        CharacterCamera *camera;

        //physics
        urchin::PhysicsWorld *physicsWorld;
        std::shared_ptr<urchin::PhysicsCharacter> physicsCharacter;
        std::unique_ptr<urchin::PhysicsCharacterController> physicsCharacterController;
        bool leftKeyPressed, rightKeyPressed, upKeyPressed, downKeyPressed;
        std::vector<urchin::GeometryModel *> rayModels;
        std::unique_ptr<urchin::CollisionPointDisplayer> collisionPointsDisplayer;

        //AI
        urchin::AIManager *aiManager;
        std::shared_ptr<urchin::PathRequest> pathRequest;
        bool displayPath;
        std::vector<urchin::GeometryModel *> pathModels;
        std::unique_ptr<urchin::NavMeshDisplayer> navMeshDisplayer;

        //GUI
        urchin::GUIRenderer *gameGUIRenderer;
        urchin::Text *fpsText;

        urchin::Text *myText;
        urchin::Button *myButton;
        urchin::TextBox *textBox;
        urchin::Window *myWindow, *myWindow2;
        urchin::Slider *mySlider;

        //sound
        urchin::ManualTrigger *manualTrigger;
};

#endif