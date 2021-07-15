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
    sceneManager.reset(nullptr);
    soundManager.reset(nullptr);

    SingletonManager::destroyAllSingletons();
}

void MainDisplayer::initialize(const std::string& resourcesDirectory) {
    FileSystem::instance().setupResourcesDirectory(resourcesDirectory);
    ConfigService::instance().loadProperties("engine.properties");
    UISkinService::instance().setSkin("ui/skinDefinition.uda");

    auto surfaceCreator = getWindowController().newSurfaceCreator();
    auto framebufferSizeRetriever = getWindowController().newFramebufferSizeRetriever();
    sceneManager = std::make_unique<SceneManager>(WindowController::windowRequiredExtensions(), std::move(surfaceCreator), std::move(framebufferSizeRetriever));
    sceneManager->updateVerticalSync(false);
    soundManager = std::make_unique<SoundManager>();

    gameRenderer = std::make_unique<GameRenderer>(this);
    gameRenderer->active(true);
}

void MainDisplayer::paint() {
    //refresh scene
    if (gameRenderer->isActive()) {
        gameRenderer->refresh();
    }

    //display the scene
    sceneManager->display();
}

void MainDisplayer::resize() {
    if (sceneManager) {
        sceneManager->onResize();
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

    sceneManager->onMouseMove(mouseX, mouseY);
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

SceneManager* MainDisplayer::getSceneManager() const {
    return sceneManager.get();
}

SoundManager* MainDisplayer::getSoundManager() const {
    return soundManager.get();
}
