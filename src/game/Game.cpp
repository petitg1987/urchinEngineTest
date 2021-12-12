#include <random>

#include <game/Game.h>
#include <game/CloseWindowCmd.h>
#include <MainContext.h>
using namespace urchin;

//debug parameters
bool DEBUG_DISPLAY_NAV_MESH = false;
bool DEBUG_DISPLAY_PATH = false;
bool DEBUG_DISPLAY_COLLISION_POINTS = false;

Game::Game(MainContext& context) :
        context(context),
        editMode(false),
        memCheckMode(false),
        //3d
        gameRenderer3d(nullptr),
        underWaterEvent(nullptr),
        //physics
        leftKeyPressed(false), rightKeyPressed(false), upKeyPressed(false), downKeyPressed(false),
        //UI
        gameUIRenderer(nullptr),
        fpsText(nullptr),
        //sound
        manualTrigger(nullptr) {
    initialize();

    context.getWindowController().setMouseCursorVisible(false);
    context.getScene().enableRenderer3d(gameRenderer3d);
    context.getScene().enableUIRenderer(gameUIRenderer);
}

Game::~Game() {
    uninitializeCharacter();
    uninitializeWaterEvent();
    uninitializeNPC();

    deleteGeometryModels(rayModels);
    deleteGeometryModels(pathModels);
}

void Game::initialize() {
    //3d
    gameRenderer3d = &context.getScene().newRenderer3d(false);
    gameRenderer3d->activateAntiAliasing(true);
    gameRenderer3d->activateAmbientOcclusion(true);
    gameRenderer3d->activateShadow(true);
    gameRenderer3d->getLightManager().setGlobalAmbientColor(Point3<float>(0.05f, 0.05f, 0.05f));
    camera = std::make_shared<CharacterCamera>(context.getWindowController(), 90.0f, 0.1f, 300.0f);
    gameRenderer3d->setCamera(camera);

    //physics
    physicsWorld = std::make_unique<PhysicsWorld>();
    collisionPointsDisplayer = std::make_unique<CollisionPointDisplayer>(*physicsWorld, gameRenderer3d);

    //AI
    aiEnvironment = std::make_unique<AIEnvironment>();
    navMeshDisplayer = std::make_unique<NavMeshDisplayer>(*aiEnvironment, *gameRenderer3d);

    //load map
    map = std::make_unique<Map>(*gameRenderer3d, *physicsWorld, context.getSoundEnvironment(), *aiEnvironment);
    LoadMapCallback nullLoadMapCallback;
    MapSaveService().loadMap("map.uda", nullLoadMapCallback, map);
    map->getObjectEntity("characterAnimate").getModel()->loadAnimation("move", "models/characterAnimate.urchinAnim");
    map->getObjectEntity("characterAnimate").getModel()->animate("move", true);

    //UI
    gameUIRenderer = &context.getScene().newUIRenderer(false);

    fpsText = Text::create(nullptr, Position(15, 4, LengthType::PIXEL), "defaultSkin", "? fps");
    gameUIRenderer->addWidget(fpsText);

    auto myFirstWindow = Window::create(nullptr, Position(30, 30, LengthType::PIXEL), Size(350, 405, LengthType::PIXEL), "defaultSkin", "window.commands.title");
    gameUIRenderer->addWidget(myFirstWindow);
    Text::create(myFirstWindow.get(), Position(0, 8, LengthType::PIXEL), "defaultSkin",
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
    TextBox::create(myFirstWindow.get(), Position(0.0f, 305.0f, LengthType::PIXEL), Size(340.0f, 30.0f, LengthType::PIXEL), "defaultSkin");
    auto myButton = Button::create(myFirstWindow.get(), Position(0.0f, 340.0f, LengthType::PIXEL), Size(340.0f, 35.0f, LengthType::PIXEL), "defaultSkin", "button.close");
    myButton->addEventListener(std::make_unique<CloseWindowCmd>(*myFirstWindow));

    auto mySecondWindow = Window::create(nullptr, Position(420.0f, 30.0f, LengthType::PIXEL), Size(200.0f, 160.0f, LengthType::PIXEL), "defaultSkin", "window.second.title");
    gameUIRenderer->addWidget(mySecondWindow);
    std::vector<std::string> sequenceValues = {"sequence.one", "sequence.two", "sequence.three"};
    Sequence::createTranslatable(mySecondWindow.get(), Position(10.0f, 4.0f, LengthType::PIXEL), Size(120.0f, 16.0f, LengthType::PIXEL), "defaultSkin", sequenceValues);
    CheckBox::create(mySecondWindow.get(), Position(10.0f, 30.0f, LengthType::PIXEL), Size(16.0f, 16.0f, LengthType::PIXEL), "defaultSkin");
    std::vector<std::string> sliderValues {"1", "2", "3"};
    Slider::create(mySecondWindow.get(), Position(10.0f, 64.0f, LengthType::PIXEL), Size(70.0f, 16.0, LengthType::PIXEL), "defaultSkin", sliderValues);

    //UI 3d
    Transform<float> transform(Point3<float>(5.0f, -1.0f, 0.0f), Quaternion<float>::rotationY(-0.80f), 1.0f);
    UIRenderer& myUi3dRenderer = gameRenderer3d->get3dUiContainer().newUI3dRenderer(transform, Point2<unsigned int>(1000, 666), Point2<float>(1.5f, 1.0f), 0.4f);
    auto my3dWindow = Window::create(nullptr, Position(0.0f, 0.0f, LengthType::SCREEN_PERCENT), Size(100, 100, LengthType::SCREEN_PERCENT), "defaultSkin", "");
    Text::create(my3dWindow.get(), Position(10.0f, 4.0f, LengthType::SCREEN_PERCENT), "default3DSkin", "UI 3d test");
    CheckBox::create(my3dWindow.get(), Position(10.0f, 20.0f, LengthType::SCREEN_PERCENT), Size(5.32f, 8.0f, LengthType::SCREEN_PERCENT), "defaultSkin");
    std::vector<std::string> slider3dValues {"1...", "2...", "3...", "4...", "5...", "6...", "7..."};
    Slider::create(my3dWindow.get(), Position(10.0f, 36.0f, LengthType::SCREEN_PERCENT), Size(50.0f, 16.0f, LengthType::SCREEN_PERCENT), "default3DSkin", slider3dValues);
    myUi3dRenderer.addWidget(my3dWindow);

    //sound
    manualTrigger = dynamic_cast<ManualTrigger*>(map->getSoundEntity("globalSound").getSoundTrigger());
    manualTrigger->playNew();

    //initialize and start process
    initializeCharacter();
    initializeWaterEvent();
    initializeNPC();
    physicsWorld->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 60.0f));
    aiEnvironment->setUp(memCheckMode ? (1.0f / 2.0f) : (1.0f / 4.0f));
    map->unpause();
}

void Game::initializeCharacter() {
    Point3<float> characterPosition(-5.0, 0.0, 15.0);
    PhysicsTransform transform(characterPosition, Quaternion<float>::rotationY(0.0f));
    float characterRadius = 0.25f;
    float characterSize = 1.80f;
    auto characterShape = std::make_unique<const CollisionCapsuleShape>(characterRadius, characterSize - (2.0f * characterRadius), CapsuleShape<float>::CAPSULE_Y);

    physicsCharacter = std::make_shared<PhysicsCharacter>("playerCharacter", 80.0f, std::move(characterShape), transform);
    characterControllerConfig = CharacterControllerConfig();
    characterController = std::make_unique<CharacterController>(physicsCharacter, characterControllerConfig, *physicsWorld);

    camera->rotate(transform.getOrientation());
}

void Game::uninitializeCharacter() {
    characterController.reset(nullptr);
    physicsCharacter = std::shared_ptr<PhysicsCharacter>(nullptr);
}

void Game::initializeWaterEvent() {
    underWaterEvent = std::make_unique<UnderWaterEvent>(context.getSoundEnvironment());

    const Water* water = map->getWaterEntity("ocean").getWater();
    water->addObserver(underWaterEvent.get(), Water::MOVE_UNDER_WATER);
    water->addObserver(underWaterEvent.get(), Water::MOVE_ABOVE_WATER);
}

void Game::uninitializeWaterEvent() {
    if (map) {
        const Water* water = map->getWaterEntity("ocean").getWater();
        water->removeObserver(underWaterEvent.get(), Water::MOVE_UNDER_WATER);
        water->removeObserver(underWaterEvent.get(), Water::MOVE_ABOVE_WATER);

        underWaterEvent.reset(nullptr);
    }
}

void Game::initializeNPC() {
    npcNavigation = std::make_unique<NPCNavigation>(5.0f, 80.0f, *map, *aiEnvironment, *physicsWorld);
}

void Game::uninitializeNPC() {
    npcNavigation.reset(nullptr);
}

void Game::switchMode() {
    editMode = !editMode;

    context.getWindowController().setMouseCursorVisible(editMode);
    camera->useMouseToMoveCamera(!editMode);
}

void Game::deleteGeometryModels(std::vector<std::shared_ptr<GeometryModel>>& models) const {
    for (const auto& model : models) {
        gameRenderer3d->getGeometryContainer().removeGeometry(*model);
    }
    models.clear();
}

void Game::onKeyPressed(Control::Key key) {
    if (key == Control::Key::E) {
        switchMode();
    }

    //3d
    static float angle = 0.0f;
    if (key == Control::Key::PAGE_UP) {
        angle += 0.1f;
        getSunLight()->setDirection(Vector3<float>(-std::sin(angle) * -1600.0f - 800.0f, -400.0f, -std::cos(angle) * 300.0f + 100.0f));
    } else if (key == Control::Key::PAGE_DOWN) {
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
        RigidBody* body = getRandomInactiveBody();
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
        Point2<float> screenCenter((float)context.getScene().getSceneWidth() / 2.0f, (float)context.getScene().getSceneHeight() / 2.0f);
        Ray<float> ray = CameraSpaceService(*gameRenderer3d->getCamera()).screenPointToRay(screenCenter, 100.0f);
        std::shared_ptr<const RayTestResult> rayTestResult = physicsWorld->rayTest(ray);

        deleteGeometryModels(rayModels);

        Vector3<float> gunRayVector = ray.getOrigin().vector(ray.computeTo());
        Point3<float> gunRayCenter = ray.getOrigin().translate(gunRayVector * 0.5f);
        Quaternion<float> gunRayOrientation = Quaternion<float>::rotationFromTo(Vector3<float>(1.0f, 0.0f, 0.0f), gunRayVector.normalize()).normalize();
        auto gunRayModel = std::make_shared<CylinderModel>(Cylinder<float>(0.01f, gunRayVector.length(), CylinderShape<float>::CYLINDER_X, gunRayCenter, gunRayOrientation), 5);
        gunRayModel->setPolygonMode(PolygonMode::FILL);
        rayModels.push_back(gunRayModel);
        gameRenderer3d->getGeometryContainer().addGeometry(std::move(gunRayModel));

        while (!rayTestResult->isResultReady()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        if (rayTestResult->hasHit()) {
            const auto& nearestResult = rayTestResult->getNearestResult();
            auto hitSphereModel = std::make_shared<SphereModel>(Sphere<float>(0.08f, nearestResult.getHitPointOnObject2()), 10);
            hitSphereModel->setPolygonMode(PolygonMode::FILL);
            hitSphereModel->setColor(1.0f, 0.0f, 0.0f);

            rayModels.push_back(hitSphereModel);
            gameRenderer3d->getGeometryContainer().addGeometry(std::move(hitSphereModel));
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
        characterController->jump();
    }

    //sound
    if (key == Control::Key::P) {
        manualTrigger->pauseAll();
    } else if (key == Control::Key::O) {
        manualTrigger->stopAll();
    } else if (key == Control::Key::M) {
        manualTrigger->playNew();
    }
}

void Game::onKeyReleased(Control::Key key) {
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

void Game::refresh() {
    //fps
    float dt = context.getScene().getDeltaTime();
    if (fpsText != nullptr) {
        fpsText->updateText(std::to_string(context.getScene().getFpsForDisplay()) + " fps");
    }

    //map
    map->refresh();

    //NPC navigation
    npcNavigation->display(context.getScene());

    //character
    characterController->setVelocity(getWalkVelocity());
    characterController->setOrientation(camera->getView());
    characterController->update(dt);
    float characterCenterToEyeDistance = 0.75f;
    camera->moveTo(physicsCharacter->getTransform().getPosition() + Point3<float>(0.0f, characterCenterToEyeDistance, 0.0f));

    //path
    if (DEBUG_DISPLAY_PATH && npcNavigation->getPathRequest() && npcNavigation->getPathRequest()->isPathReady()) {
        deleteGeometryModels(pathModels);
        std::vector<PathPoint> pathPoints = npcNavigation->getPathRequest()->getPath();
        if (!pathPoints.empty()) {
            std::vector<Sphere<float>> pathSpheres;
            pathSpheres.reserve(pathPoints.size());
            for (const auto& pathPoint : pathPoints) {
                pathSpheres.emplace_back(0.15f, pathPoint.getPoint());
            }

            auto sphereModel = std::make_shared<SphereModel>(pathSpheres, 7);
            sphereModel->setColor(0.0f, 1.0f, 1.0f);
            sphereModel->setPolygonMode(PolygonMode::FILL);
            pathModels.push_back(sphereModel);
            gameRenderer3d->getGeometryContainer().addGeometry(sphereModel);
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

SunLight* Game::getSunLight() {
    const LightEntity& sunLight = map->getLightEntity("sunLight");
    return dynamic_cast<SunLight*>(sunLight.getLight());
}

Vector3<float> Game::getWalkVelocity() const {
    Vector3<float> viewVector = camera->getView();
    viewVector.Y = 0.0f; //don't move on Y axis
    Vector3<float> forwardDirection(0.0f, 0.0f, 0.0f);

    Vector3<float> lateralVector = viewVector.crossProduct(camera->getUp());
    Vector3<float> lateralDirection(0.0f, 0.0f, 0.0f);

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

    float speed = characterControllerConfig.getMaxHorizontalSpeed();
    if (underWaterEvent->isUnderWater()) {
        speed *= 0.75f;
    }

    return (forwardDirection + lateralDirection).normalize() * speed;
}

RigidBody* Game::getRandomInactiveBody() {
    std::vector<RigidBody *> bodies;

    const auto& sceneModels = map->getObjectEntities();
    for (auto& sceneModel : sceneModels) {
        if (sceneModel->getRigidBody() && !sceneModel->getRigidBody()->isStatic() && !sceneModel->getRigidBody()->isActive()) {
            bodies.push_back(sceneModel->getRigidBody());
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
