#include <memory>

#include <ScreenHandler.h>
using namespace urchin;

ScreenHandler::ScreenHandler(WindowController& windowController) :
        windowController(windowController),
        mouseX(0),
        mouseY(0) {

}

ScreenHandler::~ScreenHandler() {
    gameRenderer.reset(nullptr);
    scene.reset(nullptr);
    soundEnvironment.reset(nullptr);

    SingletonContainer::destroyAllSingletons();
}

void ScreenHandler::initialize(const std::string& resourcesDirectory) {
    FileSystem::instance().setupResourcesDirectory(resourcesDirectory);
    ConfigService::instance().loadProperties("engine.properties");
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    auto surfaceCreator = getWindowController().newSurfaceCreator();
    auto framebufferSizeRetriever = getWindowController().newFramebufferSizeRetriever();
    scene = std::make_unique<Scene>(WindowController::windowRequiredExtensions(), std::move(surfaceCreator), std::move(framebufferSizeRetriever));
    scene->updateVerticalSync(false);
    soundEnvironment = std::make_unique<SoundEnvironment>();

    gameRenderer = std::make_unique<GameRenderer>(this);
    gameRenderer->active(true);
}

void ScreenHandler::paint() {
    //refresh scene
    if (gameRenderer->isActive()) {
        gameRenderer->refresh();
    }

    //display the scene
    scene->display();
}

void ScreenHandler::resize() {
    if (scene) {
        scene->onResize();
    }
}

void ScreenHandler::onKeyPressed(Control::Key key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyPressed(key);
    }
}

void ScreenHandler::onKeyReleased(Control::Key key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyReleased(key);
    }
}

void ScreenHandler::onMouseMove(double mouseX, double mouseY) {
    this->mouseX = mouseX;
    this->mouseY = mouseY;

    scene->onMouseMove(mouseX, mouseY);
}

double ScreenHandler::getMouseX() const {
    return mouseX;
}

double ScreenHandler::getMouseY() const {
    return mouseY;
}

WindowController& ScreenHandler::getWindowController() const {
    return windowController;
}

Scene* ScreenHandler::getScene() const {
    return scene.get();
}

SoundEnvironment* ScreenHandler::getSoundEnvironment() const {
    return soundEnvironment.get();
}
