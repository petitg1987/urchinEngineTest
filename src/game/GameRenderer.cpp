#include <random>

#include "game/GameRenderer.h"
#include "game/CloseWindowCmd.h"
#include "MainDisplayer.h"

//Debug parameters
bool DEBUG_DISPLAY_NAV_MESH = true;
bool DEBUG_DISPLAY_COLLISION_POINTS = false;

GameRenderer::GameRenderer(MainDisplayer *mainDisplayer) :
    Renderer(mainDisplayer),
    isInitialized(false),
    editMode(false),
    memCheckMode(false),
    //3d
    gameRenderer3d(nullptr),
    mapHandler(nullptr),
    camera(nullptr),
    //physics
    physicsWorld(nullptr),
    physicsCharacterController(nullptr),
    leftKeyPressed(false), rightKeyPressed(false), upKeyPressed(false), downKeyPressed(false),
    //AI
    aiManager(nullptr),
    displayPath(true),
    //GUI
    gameGUIRenderer(nullptr),
    fpsText(nullptr),
    myText(nullptr),
    myButton(nullptr),
    textBox(nullptr),
    myWindow(nullptr),
    myWindow2(nullptr),
    mySlider(nullptr),
    //sound
    manualTrigger(nullptr)
{

}

GameRenderer::~GameRenderer()
{
    uninitializeCharacter();

    deleteGeometryModels(rayModels);
    deleteGeometryModels(pathModels);

    delete mapHandler;
    delete physicsWorld;
    delete aiManager;
    delete camera;
}

void GameRenderer::initialize()
{
    //3d
    gameRenderer3d = getMainDisplayer()->getSceneManager()->newRenderer3d(false);
    gameRenderer3d->activateAntiAliasing(true);
    gameRenderer3d->activateAmbientOcclusion(true);
    gameRenderer3d->activateShadow(true);
    gameRenderer3d->getLightManager()->setGlobalAmbientColor(urchin::Point4<float>(0.05, 0.05, 0.05, 0.0));
    camera = new CharacterCamera(45.0f, 0.1f, 300.0f, getMainDisplayer()->getWindowController());
    gameRenderer3d->setCamera(camera);

    //physics
    physicsWorld = new urchin::PhysicsWorld();
    initializeCharacter();
    collisionPointsDisplayer = std::make_unique<urchin::CollisionPointDisplayer>(physicsWorld, gameRenderer3d);

    //AI
    aiManager = new urchin::AIManager();
    pathRequest = std::make_shared<urchin::PathRequest>(urchin::Point3<float>(-55.0, -2.0, -70.0), urchin::Point3<float>(55.0, -2.0, 0.0));
    aiManager->addPathRequest(pathRequest);
    navMeshDisplayer = std::make_unique<urchin::NavMeshDisplayer>(aiManager, gameRenderer3d);

    //load map
    mapHandler = new urchin::MapHandler(gameRenderer3d, physicsWorld, getMainDisplayer()->getSoundManager(), aiManager);
    urchin::NullLoadCallback nullLoadCallback;
    mapHandler->loadMapFromFile("map.xml", nullLoadCallback);

    //GUI
    gameGUIRenderer = getMainDisplayer()->getSceneManager()->newGUIRenderer(false);
    gameGUIRenderer->setupSkin("UI/skinUI.xml");

    fpsText = new urchin::Text(urchin::Position(15, 4, urchin::Position::PIXEL), "UI/font/font.fnt");
    gameGUIRenderer->addWidget(fpsText);

    myWindow = new urchin::Window(urchin::Position(30, 30, urchin::Position::PIXEL), urchin::Size(270, 250, urchin::Size::PIXEL), "defaultSkin", "Commands");
    gameGUIRenderer->addWidget(myWindow);

    myText = new urchin::Text(urchin::Position(0, 4, urchin::Position::PIXEL), "UI/font/font.fnt");
    myText->setText(
            "> Z/Q/S/D: Move character\n"
            "> Space: Jump\n"
            "> E: Edit mode (on/off)\n"
            "> F: Apply force on dynamic bodies\n"
            "> G: Launch ray test\n"
            "> C: Pause, play physics\n"
            "> PgUp/PgDn: Move light\n"
            "> A: Anti aliasing (on/off)\n"
            "> H: HBAO (on/off)\n"
            "> W: Shadow (on/off)\n"
            "> P/M/O: Pause, play, stop sound\n"
            "> K: Display path (on/off)\n"
            "> Esc: Quit\n", 235);
    myWindow->addChild(myText);

    myButton = new urchin::Button(urchin::Position(0, 195, urchin::Position::PIXEL), urchin::Size(264, 27, urchin::Size::PIXEL), "defaultSkin", "Fermer");
    myButton->addEventListener(std::make_shared<CloseWindowCmd>(myWindow));
    myWindow->addChild(myButton);

    textBox = new urchin::TextBox(urchin::Position(0, 172, urchin::Position::PIXEL), urchin::Size(264, 20, urchin::Size::PIXEL), "defaultSkin");
    myWindow->addChild(textBox);

    /*myWindow2 = new urchin::Window(urchin::Position(320, 30, urchin::Position::PIXEL), urchin::Size(200, 160, urchin::Size::PIXEL), "defaultSkin", "Second Windows");
    gameGUIRenderer->addWidget(myWindow2);

    std::vector<std::string> values = {"One", "Two", "Three", "Four"};
    mySlider = new urchin::Slider(urchin::Position(10, 4, urchin::Position::PIXEL), urchin::Size(70, 16, urchin::Size::PIXEL), values, "defaultSkin");
    myWindow2->addChild(mySlider); */

    //sound
    manualTrigger = dynamic_cast<urchin::ManualTrigger *>(mapHandler->getMap()->getSceneSound("ambientSound")->getSoundTrigger());
    manualTrigger->play();

    //start process
    physicsWorld->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 60.0f));
    aiManager->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 4.0f));
    mapHandler->unpause();

    isInitialized = true;
}

void GameRenderer::initializeCharacter()
{
    urchin::Point3<float> characterPosition(-5.0, 0.0, 15.0);
    urchin::PhysicsTransform transform(characterPosition, urchin::Quaternion<float>(urchin::Vector3<float>(0.0, 1.0, 0.0), 0.0));
    float characterRadius = 0.25f;
    float characterSize = 1.80f;
    auto characterShape = std::make_shared<const urchin::CollisionCapsuleShape>(characterRadius, characterSize-(2.0f*characterRadius), urchin::CapsuleShape<float>::CAPSULE_Y);

    physicsCharacter = std::make_shared<urchin::PhysicsCharacter>("playerCharacter", 80.0f, characterShape, transform);
    physicsCharacterController = std::make_unique<urchin::PhysicsCharacterController>(physicsCharacter, physicsWorld);

    camera->rotate(transform.getOrientation());
}

void GameRenderer::uninitializeCharacter()
{
    physicsCharacterController.reset(nullptr);
    physicsCharacter = std::shared_ptr<urchin::PhysicsCharacter>(nullptr);
}

void GameRenderer::switchMode()
{
    editMode = !editMode;

    getMainDisplayer()->getWindowController()->setMouseCursorVisible(editMode);
    camera->useMouseToMoveCamera(!editMode);
}

void GameRenderer::deleteGeometryModels(std::vector<urchin::GeometryModel *> &models) const
{
    for (auto model : models)
    {
        gameRenderer3d->getGeometryManager()->removeGeometry(model);
        delete model;
    }
    models.clear();
}

void GameRenderer::onKeyPressed(KeyboardKey key)
{
    if(key == KeyboardKey::E)
    {
        switchMode();
    }

    //3d
    static float angle=0.0f;
    if(key == KeyboardKey::PAGE_UP)
    {
        angle+=0.1;
        getSunLight()->setDirection(urchin::Vector3<float>(-std::sin(angle)*-1600.0-800.0, -400.0, -std::cos(angle)*300.0+100.0));
    }if(key == KeyboardKey::PAGE_DOWN)
    {
        angle-=0.1;
        getSunLight()->setDirection(urchin::Vector3<float>(-std::sin(angle)*-1600.0-800.0, -400.0, -std::cos(angle)*300.0+100.0));
    }

    static bool aliasingActive = true;
    if(key == KeyboardKey::A)
    {
        aliasingActive = !aliasingActive;
        gameRenderer3d->activateAntiAliasing(aliasingActive);
    }

    static bool hbaoActive = true;
    if(key == KeyboardKey::H)
    {
        hbaoActive = !hbaoActive;
        gameRenderer3d->activateAmbientOcclusion(hbaoActive);
    }

    static bool shadowActive = true;
    if(key == KeyboardKey::W)
    {
        shadowActive = !shadowActive;
        gameRenderer3d->activateShadow(shadowActive);
    }

    //physics
    if(key == KeyboardKey::F)
    {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<> distribution(0, 10);

        int xAxisForce = distribution(generator) - 5;
        int zAxisForce = distribution(generator) - 5;
        int yAxisForce = distribution(generator) + 5;
        urchin::RigidBody *body = getRandomUnactiveBody();
        if(body!=nullptr)
        {
            body->applyCentralMomentum(urchin::Vector3<float>((float)xAxisForce, body->getMass() * (float)yAxisForce, (float)zAxisForce));
        }
    }else if(key == KeyboardKey::C)
    {
        if(physicsWorld->isPaused())
        {
            physicsWorld->unpause();
        }else
        {
            physicsWorld->pause();
        }
    }else if(key == KeyboardKey::G)
    {
        float clipSpaceX = (2.0f * (float)getMainDisplayer()->getMouseX()) / ((float)getMainDisplayer()->getSceneManager()->getSceneWidth()) - 1.0f;
        float clipSpaceY = 1.0f - (2.0f * (float)getMainDisplayer()->getMouseY()) / ((float)getMainDisplayer()->getSceneManager()->getSceneHeight());
        urchin::Vector4<float> rayDirectionClipSpace(clipSpaceX, clipSpaceY, -1.0f, 1.0f);
        urchin::Vector4<float> rayDirectionEyeSpace = camera->getProjectionMatrix().inverse() * rayDirectionClipSpace;
        rayDirectionEyeSpace.setValues(rayDirectionEyeSpace.X, rayDirectionEyeSpace.Y, -1.0f, 0.0f);
        urchin::Vector3<float> rayDirectionWorldSpace = (camera->getViewMatrix().inverse() * rayDirectionEyeSpace).xyz().normalize();

        urchin::Point3<float> rayStart = camera->getPosition().translate(rayDirectionWorldSpace*1.00f);
        urchin::Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace*100.0f);
        std::shared_ptr<const urchin::RayTestResult> rayTestResult = physicsWorld->rayTest(urchin::Ray<float>(rayStart, rayEnd));

        urchin::LineSegment3D<float> gunRay(rayStart, rayEnd);
        urchin::GeometryModel *gunRayModel = new urchin::LinesModel(gunRay, 3.0f);
        deleteGeometryModels(rayModels);
        rayModels.push_back(gunRayModel);
        gameRenderer3d->getGeometryManager()->addGeometry(gunRayModel);

        while(!rayTestResult->isResultReady())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if(rayTestResult->hasHit())
        {
            const auto &nearestResult = rayTestResult->getNearestResult();
            urchin::GeometryModel *hitPointModel = new urchin::PointsModel(nearestResult->getHitPointOnObject2(), 25.0);
            hitPointModel->setColor(1.0, 0.0, 0.0);

            rayModels.push_back(hitPointModel);
            gameRenderer3d->getGeometryManager()->addGeometry(hitPointModel);
        }
    }

    if(key == KeyboardKey::Q)
    {
        leftKeyPressed = true;
    }else if(key == KeyboardKey::D)
    {
        rightKeyPressed = true;
    }else if(key == KeyboardKey::Z)
    {
        upKeyPressed = true;
    }else if(key == KeyboardKey::S)
    {
        downKeyPressed = true;
    }else if(key == KeyboardKey::SPACE)
    {
        physicsCharacterController->jump();
    }

    //AI
    if(key == KeyboardKey::K)
    {
        deleteGeometryModels(pathModels);
        displayPath = !displayPath;
    }

    //sound
    if(key == KeyboardKey::P)
    {
        manualTrigger->pause();
    }else if(key == KeyboardKey::O)
    {
        manualTrigger->stop();
    }else if(key == KeyboardKey::M)
    {
        manualTrigger->play();
    }
}

void GameRenderer::onKeyReleased(KeyboardKey key)
{
    //3d
    if(key == KeyboardKey::Q)
    {
        leftKeyPressed = false;
    }else if(key == KeyboardKey::D)
    {
        rightKeyPressed = false;
    }else if(key == KeyboardKey::Z)
    {
        upKeyPressed = false;
    }else if(key == KeyboardKey::S)
    {
        downKeyPressed = false;
    }
}

void GameRenderer::active(bool active)
{
    if(active)
    {
        if(!isInitialized)
        {
            initialize();
        }

        getMainDisplayer()->getWindowController()->setMouseCursorVisible(false);
        getMainDisplayer()->getWindowController()->setVerticalSyncEnabled(false);

        getMainDisplayer()->getSceneManager()->enableRenderer3d(gameRenderer3d);
        getMainDisplayer()->getSceneManager()->enableGUIRenderer(gameGUIRenderer);
    }else
    {
        getMainDisplayer()->getSceneManager()->enableRenderer3d(nullptr);
        getMainDisplayer()->getSceneManager()->enableGUIRenderer(nullptr);
    }
}

bool GameRenderer::isActive() const
{
    return gameRenderer3d!=nullptr && getMainDisplayer()->getSceneManager()->getActiveRenderer3d()==gameRenderer3d;
}

void GameRenderer::refresh()
{
    //fps
    float dt = getMainDisplayer()->getSceneManager()->getDeltaTime();
    if(fpsText!=nullptr)
    {
        fpsText->setText(std::to_string(getMainDisplayer()->getSceneManager()->getFpsForDisplay()) + " fps");
    }

    //map
    mapHandler->refreshMap();

    //character
    physicsCharacterController->setMomentum(getWalkMomentum());
    physicsCharacterController->update(dt);
    camera->moveTo(physicsCharacter->getTransform().getPosition() + urchin::Point3<float>(0.0, 0.75f, 0.0));

    //path
    if(displayPath)
    {
        deleteGeometryModels(pathModels);
        std::vector<urchin::PathPoint> pathPoints = pathRequest->getPath();

        if (pathPoints.size() >= 2 && displayPath)
        {
            std::vector<urchin::Point3<float>> adjustedPathPoints;
            adjustedPathPoints.reserve(pathPoints.size());
            for (auto &pathPoint : pathPoints)
            {
                adjustedPathPoints.emplace_back(urchin::Point3<float>(pathPoint.getPoint().X, pathPoint.getPoint().Y + 0.02f, pathPoint.getPoint().Z));
            }
            auto *linesModel = new urchin::LinesModel(adjustedPathPoints, 5.0);
            linesModel->setColor(0.0, 1.0, 1.0, 1.0);
            pathModels.push_back(linesModel);
            gameRenderer3d->getGeometryManager()->addGeometry(linesModel);
        }
    }

    if(memCheckMode)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if(DEBUG_DISPLAY_NAV_MESH && navMeshDisplayer)
    {
        navMeshDisplayer->display();
    }

    if(DEBUG_DISPLAY_COLLISION_POINTS && navMeshDisplayer)
    {
        navMeshDisplayer->display();
    }
}

urchin::SunLight *GameRenderer::getSunLight()
{
    urchin::SceneLight *sunLight = mapHandler->getMap()->getSceneLight("sunLight");
    return dynamic_cast<urchin::SunLight *>(sunLight->getLight());
}

urchin::Vector3<float> GameRenderer::getWalkMomentum() const
{
    urchin::Vector3<float> viewVector = camera->getView();
    viewVector.Y = 0.0f; //don't move on Y axis
    urchin::Vector3<float> forwardDirection(0.0, 0.0, 0.0);

    urchin::Vector3<float> lateralVector = viewVector.crossProduct(camera->getUp());
    urchin::Vector3<float> lateralDirection(0.0, 0.0, 0.0);

    if(upKeyPressed && !downKeyPressed)
    {
        forwardDirection = viewVector.normalize();
    }else if(downKeyPressed && !upKeyPressed)
    {
        forwardDirection = -viewVector.normalize();
    }

    if(leftKeyPressed && !rightKeyPressed)
    {
        lateralDirection = -lateralVector.normalize();
    }else if(rightKeyPressed && !leftKeyPressed)
    {
        lateralDirection = lateralVector.normalize();
    }

    const float speed = 1500.0f;
    return (forwardDirection + lateralDirection).normalize() * speed;
}

urchin::RigidBody *GameRenderer::getRandomUnactiveBody()
{
    std::vector<urchin::RigidBody *> bodies;

    const std::list<urchin::SceneObject *> &sceneObjects = mapHandler->getMap()->getSceneObjects();
    for (auto sceneObject : sceneObjects)
    {
        if(!sceneObject->getRigidBody()->isStatic() && !sceneObject->getRigidBody()->isActive())
        {
            bodies.push_back(sceneObject->getRigidBody());
        }
    }

    if(!bodies.empty())
    {
        auto seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<> distribution(0, bodies.size()-1);
        return bodies[distribution(generator)];
    }

    return nullptr;
}
