#include <memory>

#include "MainDisplayer.h"
using namespace urchin;

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

    SingletonManager::destroyAllSingletons();
}

void MainDisplayer::initialize(const std::string& resourcesDirectory) {
    FileSystem::instance()->setupResourcesDirectory(resourcesDirectory);

    ConfigService::instance()->loadProperties("engine.properties");
    sceneManager = new SceneManager();
    soundManager = new SoundManager();

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

SceneManager* MainDisplayer::getSceneManager() const {
    return sceneManager;
}

SoundManager* MainDisplayer::getSoundManager() const {
    return soundManager;
}
