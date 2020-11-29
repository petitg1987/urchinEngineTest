#include <memory>

#include "MainDisplayer.h"

MainDisplayer::MainDisplayer(WindowController* windowController) :
    windowController(windowController),
    mouseX(0),
    mouseY(0),
    sceneManager(nullptr),
    soundManager(nullptr),
    gameRenderer(nullptr) {

}

MainDisplayer::~MainDisplayer() {
    delete gameRenderer;

    delete sceneManager;
    delete soundManager;

    urchin::SingletonManager::destroyAllSingletons();
}

void MainDisplayer::initialize(const std::string& resourcesDirectory, const std::string& saveDirectory) {
    urchin::FileSystem::instance()->setupResourcesDirectory(resourcesDirectory);
    urchin::FileSystem::instance()->setupSaveDirectory(saveDirectory);

    urchin::ConfigService::instance()->loadProperties("engine.properties");
    sceneManager = new urchin::SceneManager();
    soundManager = new urchin::SoundManager();

    gameRenderer = new GameRenderer(this);
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

void MainDisplayer::resize(unsigned int width, unsigned int height) {
    if (sceneManager) {
        sceneManager->onResize(width, height);
    }
}

void MainDisplayer::onKeyPressed(KeyboardKey key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyPressed(key);
    }
}

void MainDisplayer::onKeyReleased(KeyboardKey key) {
    if (gameRenderer->isActive()) {
        gameRenderer->onKeyReleased(key);
    }
}

void MainDisplayer::onMouseMove(int mouseX, int mouseY) {
    this->mouseX = mouseX;
    this->mouseY = mouseY;

    sceneManager->onMouseMove(mouseX, mouseY);
}

int MainDisplayer::getMouseX() const {
    return mouseX;
}

int MainDisplayer::getMouseY() const {
    return mouseY;
}

WindowController* MainDisplayer::getWindowController() const {
    return windowController;
}

urchin::SceneManager* MainDisplayer::getSceneManager() const {
    return sceneManager;
}

urchin::SoundManager* MainDisplayer::getSoundManager() const {
    return soundManager;
}
