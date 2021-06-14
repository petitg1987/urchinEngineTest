#include <random>

#include <game/GameRenderer.h>
#include <game/CloseWindowCmd.h>
#include <MainDisplayer.h>
using namespace urchin;

//debug parameters
bool DEBUG_DISPLAY_NAV_MESH = false;
bool DEBUG_DISPLAY_PATH = false;
bool DEBUG_DISPLAY_COLLISION_POINTS = false;

GameRenderer::GameRenderer(MainDisplayer* mainDisplayer) :
        RenderScreen(mainDisplayer),
        isInitialized(false),
        editMode(false),
        memCheckMode(false),
        //3d
        gameRenderer3d(nullptr),
        mapHandler(nullptr),
        camera(nullptr),
        underWaterEvent(nullptr),
        //physics
        physicsWorld(nullptr),
        physicsCharacterController(nullptr),
        leftKeyPressed(false), rightKeyPressed(false), upKeyPressed(false), downKeyPressed(false),
        //AI
        aiManager(nullptr),
        //UI
        gameUIRenderer(nullptr),
        fpsText(nullptr),
        myText(nullptr),
        myButton(nullptr),
        textBox(nullptr),
        myWindow(nullptr),
        myWindow2(nullptr),
        mySlider(nullptr),
        //sound
        manualTrigger(nullptr) {

}

GameRenderer::~GameRenderer() {
    uninitializeCharacter();
    uninitializeWaterEvent();
    uninitializeNPC();

    deleteGeometryModels(rayModels);
    deleteGeometryModels(pathModels);

    delete mapHandler;
    delete physicsWorld;
    delete aiManager;
    delete camera;
}

void GameRenderer::initialize() {
    //3d
    gameRenderer3d = getMainDisplayer()->getSceneManager()->newRenderer3d(false);
    gameRenderer3d->activateAntiAliasing(true);
    gameRenderer3d->activateAmbientOcclusion(true);
    gameRenderer3d->activateShadow(true);
    gameRenderer3d->getLightManager()->setGlobalAmbientColor(Point4<float>(0.05f, 0.05f, 0.05f, 0.0f));
    camera = new CharacterCamera(45.0f, 0.1f, 300.0f, getMainDisplayer()->getWindowController());
    gameRenderer3d->setCamera(camera);

    //physics
    physicsWorld = new PhysicsWorld();
    collisionPointsDisplayer = std::make_unique<CollisionPointDisplayer>(physicsWorld, gameRenderer3d);

    //AI
    aiManager = new AIManager();
    navMeshDisplayer = std::make_unique<NavMeshDisplayer>(aiManager, gameRenderer3d);

    //load map
    mapHandler = new MapHandler(gameRenderer3d, physicsWorld, getMainDisplayer()->getSoundManager(), aiManager);
    NullLoadCallback nullLoadCallback;
    mapHandler->loadMapFromFile("map.xml", nullLoadCallback);
    mapHandler->getMap()->getSceneObject("characterAnimate")->getModel()->loadAnimation("move", "models/characterAnimate.urchinAnim");
    mapHandler->getMap()->getSceneObject("characterAnimate")->getModel()->animate("move");

    //UI
    gameUIRenderer = getMainDisplayer()->getSceneManager()->newUIRenderer(false);

    fpsText = Text::newText(nullptr, Position(15, 4, LengthType::PIXEL), "defaultSkin", "? fps");
    gameUIRenderer->addWidget(fpsText);

    myWindow = new Window(nullptr, Position(30, 30, LengthType::PIXEL), Size(270, 270, LengthType::PIXEL), "defaultSkin", "window.commands.title");
    gameUIRenderer->addWidget(myWindow);
    myText = Text::newText(myWindow, Position(0, 4, LengthType::PIXEL), "defaultSkin",
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
                      "> Esc: Quit");
    textBox = new TextBox(myWindow, Position(0, 192, LengthType::PIXEL), Size(264, 20, LengthType::PIXEL), "defaultSkin");
    myButton = new Button(myWindow, Position(0, 215, LengthType::PIXEL), Size(264, 27, LengthType::PIXEL), "defaultSkin", "button.close");
    myButton->addEventListener(std::make_shared<CloseWindowCmd>(myWindow));

    /*myWindow2 = new Window(nullptr, Position(320, 30, LengthType::PIXEL), Size(200, 160, LengthType::PIXEL), "defaultSkin", "window.second.title");
    gameUIRenderer->addWidget(myWindow2);
    std::vector<std::string> values = {"slider.one", "slider.two", "slider.three"};
    mySlider = Slider::newTranslatableSlider(myWindow2, Position(10, 4, LengthType::PIXEL), Size(70, 16, LengthType::PIXEL), "defaultSkin", values);*/

    //sound
    manualTrigger = dynamic_cast<ManualTrigger *>(mapHandler->getMap()->getSceneSound("globalSound")->getSoundTrigger());
    manualTrigger->play();

    //initialize and start process
    initializeCharacter();
    initializeWaterEvent();
    initializeNPC();
    physicsWorld->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 60.0f));
    aiManager->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 4.0f));
    mapHandler->unpause();

    isInitialized = true;
}

void GameRenderer::initializeCharacter() {
    Point3<float> characterPosition(-5.0, 0.0, 15.0);
    PhysicsTransform transform(characterPosition, Quaternion<float>(Vector3<float>(0.0, 1.0, 0.0), 0.0));
    float characterRadius = 0.25f;
    float characterSize = 1.80f;
    auto characterShape = std::make_shared<const CollisionCapsuleShape>(characterRadius, characterSize-(2.0f*characterRadius), CapsuleShape<float>::CAPSULE_Y);

    physicsCharacter = std::make_shared<PhysicsCharacter>("playerCharacter", 80.0f, characterShape, transform);
    physicsCharacterController = std::make_unique<PhysicsCharacterController>(physicsCharacter, physicsWorld);

    camera->rotate(transform.getOrientation());
}

void GameRenderer::uninitializeCharacter() {
    physicsCharacterController.reset(nullptr);
    physicsCharacter = std::shared_ptr<PhysicsCharacter>(nullptr);
}

void GameRenderer::initializeWaterEvent() {
    underWaterEvent = new UnderWaterEvent(getMainDisplayer()->getSoundManager());

    Water* water = mapHandler->getMap()->getSceneWater("ocean")->getWater();
    water->addObserver(underWaterEvent, Water::MOVE_UNDER_WATER);
    water->addObserver(underWaterEvent, Water::MOVE_ABOVE_WATER);
}

void GameRenderer::uninitializeWaterEvent() {
    if (mapHandler) {
        Water* water = mapHandler->getMap()->getSceneWater("ocean")->getWater();
        water->removeObserver(underWaterEvent, Water::MOVE_UNDER_WATER);
        water->removeObserver(underWaterEvent, Water::MOVE_ABOVE_WATER);

        delete underWaterEvent;
        underWaterEvent = nullptr;
    }
}

void GameRenderer::initializeNPC() {
    npcNavigation = std::make_unique<NPCNavigation>(5.0f, 80.0f, mapHandler, aiManager, physicsWorld);
}

void GameRenderer::uninitializeNPC() {
    npcNavigation.reset(nullptr);
}

void GameRenderer::switchMode() {
    editMode = !editMode;

    getMainDisplayer()->getWindowController()->setMouseCursorVisible(editMode);
    camera->useMouseToMoveCamera(!editMode);
}

void GameRenderer::deleteGeometryModels(std::vector<GeometryModel *>& models) const {
    for (auto model : models) {
        gameRenderer3d->getGeometryManager()->removeGeometry(model);
        delete model;
    }
    models.clear();
}

void GameRenderer::onKeyPressed(Control::Key key) {
    if (key == Control::Key::E) {
        switchMode();
    }

    //3d
    static float angle = 0.0f;
    if (key == Control::Key::PAGE_UP) {
        angle += 0.1f;
        getSunLight()->setDirection(Vector3<float>(-std::sin(angle) * -1600.0f - 800.0f, -400.0f, -std::cos(angle) * 300.0f + 100.0f));
    }if (key == Control::Key::PAGE_DOWN) {
        angle -= 0.1f;
        getSunLight()->setDirection(Vector3<float>(-std::sin(angle) * -1600.0f - 800.0f, -400.0f, -std::cos(angle) * 300.0f + 100.0f));
    }

    static bool aliasingActive = true;
    if (key == Control::Key::A) {
        aliasingActive = !aliasingActive;
        gameRenderer3d->activateAntiAliasing(aliasingActive);
    }

    static bool hbaoActive = true;
    if (key == Control::Key::H) {
        hbaoActive = !hbaoActive;
        gameRenderer3d->activateAmbientOcclusion(hbaoActive);
    }

    static bool shadowActive = true;
    if (key == Control::Key::W) {
        shadowActive = !shadowActive;
        gameRenderer3d->activateShadow(shadowActive);
    }

    //physics
    if (key == Control::Key::F) {
        auto seed = static_cast<std::default_random_engine::result_type>(std::chrono::system_clock::now().time_since_epoch().count());
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<> distribution(0, 10);

        int xAxisForce = distribution(generator) - 5;
        int zAxisForce = distribution(generator) - 5;
        int yAxisForce = distribution(generator) + 5;
        RigidBody* body = getRandomUnactiveBody();
        if (body != nullptr) {
            body->applyCentralMomentum(Vector3<float>((float)xAxisForce, body->getMass() * (float)yAxisForce, (float)zAxisForce));
        }
    } else if (key == Control::Key::C) {
        if (physicsWorld->isPaused()) {
            physicsWorld->unpause();
        } else {
            physicsWorld->pause();
        }
    } else if (key == Control::Key::G) {
        float clipSpaceX = (2.0f * (float)getMainDisplayer()->getMouseX()) / ((float)getMainDisplayer()->getSceneManager()->getSceneWidth()) - 1.0f;
        float clipSpaceY = 1.0f - (2.0f * (float)getMainDisplayer()->getMouseY()) / ((float)getMainDisplayer()->getSceneManager()->getSceneHeight());
        Vector4<float> rayDirectionClipSpace(clipSpaceX, clipSpaceY, -1.0f, 1.0f);
        Vector4<float> rayDirectionEyeSpace = camera->getProjectionMatrix().inverse() * rayDirectionClipSpace;
        rayDirectionEyeSpace.setValues(rayDirectionEyeSpace.X, rayDirectionEyeSpace.Y, -1.0f, 0.0f);
        Vector3<float> rayDirectionWorldSpace = (camera->getViewMatrix().inverse() * rayDirectionEyeSpace).xyz().normalize();

        Point3<float> rayStart = camera->getPosition().translate(rayDirectionWorldSpace*1.00f);
        Point3<float> rayEnd = rayStart.translate(rayDirectionWorldSpace*100.0f);
        std::shared_ptr<const RayTestResult> rayTestResult = physicsWorld->rayTest(Ray<float>(rayStart, rayEnd));

        LineSegment3D<float> gunRay(rayStart, rayEnd);
        GeometryModel* gunRayModel = new LinesModel(gunRay);
        gunRayModel->setLineWidth(3.0f);
        deleteGeometryModels(rayModels);
        rayModels.push_back(gunRayModel);
        gameRenderer3d->getGeometryManager()->addGeometry(gunRayModel);

        while (!rayTestResult->isResultReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if (rayTestResult->hasHit()) {
            const auto& nearestResult = rayTestResult->getNearestResult();
            GeometryModel* hitPointModel = new PointsModel(nearestResult->getHitPointOnObject2());
            hitPointModel->setPointSize(25.0f);
            hitPointModel->setColor(1.0, 0.0, 0.0);

            rayModels.push_back(hitPointModel);
            gameRenderer3d->getGeometryManager()->addGeometry(hitPointModel);
        }
    }

    if (key == Control::Key::Q) {
        leftKeyPressed = true;
    } else if (key == Control::Key::D) {
        rightKeyPressed = true;
    } else if (key == Control::Key::Z) {
        upKeyPressed = true;
    } else if (key == Control::Key::S) {
        downKeyPressed = true;
    } else if (key == Control::Key::SPACE) {
        physicsCharacterController->jump();
    }

    //sound
    if (key == Control::Key::P) {
        manualTrigger->pause();
    } else if (key == Control::Key::O) {
        manualTrigger->stop();
    } else if (key == Control::Key::M) {
        manualTrigger->play();
    }
}

void GameRenderer::onKeyReleased(Control::Key key) {
    //3d
    if (key == Control::Key::Q) {
        leftKeyPressed = false;
    } else if (key == Control::Key::D) {
        rightKeyPressed = false;
    } else if (key == Control::Key::Z) {
        upKeyPressed = false;
    } else if (key == Control::Key::S) {
        downKeyPressed = false;
    }
}

void GameRenderer::active(bool active) {
    if (active) {
        if (!isInitialized) {
            initialize();
        }

        getMainDisplayer()->getWindowController()->setMouseCursorVisible(false);

        getMainDisplayer()->getSceneManager()->enableRenderer3d(gameRenderer3d);
        getMainDisplayer()->getSceneManager()->enableUIRenderer(gameUIRenderer);
    } else {
        getMainDisplayer()->getSceneManager()->enableRenderer3d(nullptr);
        getMainDisplayer()->getSceneManager()->enableUIRenderer(nullptr);
    }
}

bool GameRenderer::isActive() const {
    return gameRenderer3d != nullptr && getMainDisplayer()->getSceneManager()->getActiveRenderer3d() == gameRenderer3d;
}

void GameRenderer::refresh() {
    //fps
    float dt = getMainDisplayer()->getSceneManager()->getDeltaTime();
    if (fpsText != nullptr) {
        fpsText->updateText(std::to_string(getMainDisplayer()->getSceneManager()->getFpsForDisplay()) + " fps");
    }

    //map
    mapHandler->refreshMap();

    //NPC navigation
    npcNavigation->display(getMainDisplayer());

    //character
    physicsCharacterController->setMomentum(getWalkMomentum());
    physicsCharacterController->update(dt);
    camera->moveTo(physicsCharacter->getTransform().getPosition() + Point3<float>(0.0, 0.75f, 0.0));

    //path
    if (DEBUG_DISPLAY_PATH) {
        deleteGeometryModels(pathModels);
        std::vector<PathPoint> pathPoints;
        if(npcNavigation->getPathRequest()) {
            pathPoints = npcNavigation->getPathRequest()->getPath();
        }

        if (pathPoints.size() >= 2) {
            std::vector<Point3<float>> adjustedPathPoints;
            adjustedPathPoints.reserve(pathPoints.size());
            for (auto& pathPoint : pathPoints) {
                adjustedPathPoints.emplace_back(Point3<float>(pathPoint.getPoint().X, pathPoint.getPoint().Y + 0.02f, pathPoint.getPoint().Z));
            }
            auto* linesModel = new LinesModel(adjustedPathPoints);
            linesModel->setLineWidth(5.0f);
            linesModel->setColor(0.0, 1.0, 1.0, 1.0);
            pathModels.push_back(linesModel);
            gameRenderer3d->getGeometryManager()->addGeometry(linesModel);
        }
    }

    if (memCheckMode) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (DEBUG_DISPLAY_NAV_MESH && navMeshDisplayer) {
        navMeshDisplayer->display();
    }

    if (DEBUG_DISPLAY_COLLISION_POINTS && collisionPointsDisplayer) {
        collisionPointsDisplayer->display();
    }
}

SunLight* GameRenderer::getSunLight() {
    SceneLight* sunLight = mapHandler->getMap()->getSceneLight("sunLight");
    return dynamic_cast<SunLight *>(sunLight->getLight());
}

Vector3<float> GameRenderer::getWalkMomentum() const {
    Vector3<float> viewVector = camera->getView();
    viewVector.Y = 0.0f; //don't move on Y axis
    Vector3<float> forwardDirection(0.0, 0.0, 0.0);

    Vector3<float> lateralVector = viewVector.crossProduct(camera->getUp());
    Vector3<float> lateralDirection(0.0, 0.0, 0.0);

    if (upKeyPressed && !downKeyPressed) {
        forwardDirection = viewVector.normalize();
    } else if (downKeyPressed && !upKeyPressed) {
        forwardDirection = -viewVector.normalize();
    }

    if (leftKeyPressed && !rightKeyPressed) {
        lateralDirection = -lateralVector.normalize();
    } else if (rightKeyPressed && !leftKeyPressed) {
        lateralDirection = lateralVector.normalize();
    }

    float speed = 1500.0f;
    if (underWaterEvent->isUnderWater()) {
        speed *= 0.75f;
    }

    return (forwardDirection + lateralDirection).normalize() * speed;
}

RigidBody* GameRenderer::getRandomUnactiveBody() {
    std::vector<RigidBody *> bodies;

    const std::list<SceneObject *>& sceneObjects = mapHandler->getMap()->getSceneObjects();
    for (auto sceneObject : sceneObjects) {
        if (sceneObject->getRigidBody() && !sceneObject->getRigidBody()->isStatic() && !sceneObject->getRigidBody()->isActive()) {
            bodies.push_back(sceneObject->getRigidBody());
        }
    }

    if (!bodies.empty()) {
        auto seed = static_cast<std::default_random_engine::result_type>(std::chrono::system_clock::now().time_since_epoch().count());
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<> distribution(0, (int)bodies.size() - 1);
        return bodies[static_cast<std::size_t>(distribution(generator))];
    }

    return nullptr;
}
