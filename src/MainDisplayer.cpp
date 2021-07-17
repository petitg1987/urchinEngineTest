#include <memory>

#include <MainDisplayer.h>
using namespace urchin;

MainDisplayer::MainDisplayer(WindowController& windowController) :
        windowController(windowController),
        mouseX(0),
        mouseY(0) {

}

MainDisplayer::~MainDisplayer() {
    gameRenderer.reset(nullptr);
    scene.reset(nullptr);
    soundEnvironment.reset(nullptr);

    SingletonContainer::destroyAllSingletons();
}

void MainDisplayer::initialize(const std::string& resourcesDirectory) {
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

void MainDisplayer::paint() {
    //refresh scene
    if (gameRenderer->isActive()) {
        gameRenderer->refresh();
    }

    //display the scene
    scene->display();
}

void MainDisplayer::resize() {
    if (scene) {
        scene->onResize();
    }
}

void MainDisplayer::onKeyPressed(Control::Key key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyPressed(key);
    }
}

void MainDisplayer::onKeyReleased(Control::Key key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyReleased(key);
    }
}

void MainDisplayer::onMouseMove(double mouseX, double mouseY) {
    this->mouseX = mouseX;
    this->mouseY = mouseY;

    scene->onMouseMove(mouseX, mouseY);
}

double MainDisplayer::getMouseX() const {
    return mouseX;
}

double MainDisplayer::getMouseY() const {
    return mouseY;
}

WindowController& MainDisplayer::getWindowController() const {
    return windowController;
}

Scene* MainDisplayer::getScene() const {
    return scene.get();
}

SoundEnvironment* MainDisplayer::getSoundEnvironment() const {
    return soundEnvironment.get();
}
