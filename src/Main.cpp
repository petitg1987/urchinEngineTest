#include "Main.h"
#include "KeyboardKey.h"
#include "WindowController.h"

int main(int argc, char *argv[]) {
    Main main;
    main.execute(argc, argv);

    return 0;
}

//static
std::string Main::glfwError;

Main::Main() :
        mainDisplayer(nullptr),
        windowController(nullptr),
        propagatePressKeyEvent(true),
        propagateReleaseKeyEvent(true) {

}

void Main::execute(int argc, char *argv[]) {
    initializeKeyboardMap();

    urchin::Logger::defineLogger(std::make_unique<urchin::FileLogger>("urchinEngineTest.log"));

    GLFWwindow* window = nullptr;

    try {
        if (!glfwInit()) {
            throw std::runtime_error("Impossible to initialize GLFW library");
        }
        glfwSetErrorCallback(glfwErrorCallback);

        window = createWindow(argc, argv);
        windowController = new WindowController(window);
        std::string resourcesDirectory = retrieveResourcesDirectory(argv);
        std::string saveDirectory = retrieveSaveDirectory(argv);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        mainDisplayer = new MainDisplayer(windowController);
        mainDisplayer->initialize(resourcesDirectory, saveDirectory);
        mainDisplayer->resize(width, height);

        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetCharCallback(window, charCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseKeyCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetWindowSizeCallback(window, windowSizeCallback);

        windowController->cleanEvents(); //ignore events occurred during initialization phase

        while (!glfwWindowShouldClose(window)) {
            if (!glfwError.empty()) {
                throw std::runtime_error(glfwError);
            }

            handleInputEvents();

            mainDisplayer->paint();
            glfwSwapBuffers(window);
        }

        if (urchin::Logger::logger().hasFailure()) {
            failureExit(window, windowController);
        } else {
            clearResources(window, windowController);
        }
    } catch (std::exception& e) {
        urchin::Logger::logger().logError("Error occurred: " + std::string(e.what()));
        failureExit(window, windowController);
    }
}

void Main::glfwErrorCallback(int error, const char* description) {
    if (glfwError.empty()) {
        glfwError = "GLFW error (code: " + std::to_string(error) + "): " + description;
    }
}

void Main::initializeKeyboardMap() {
    keyboardMap[GLFW_KEY_ESCAPE] = KeyboardKey::ESCAPE;
    keyboardMap[GLFW_KEY_SPACE] = KeyboardKey::SPACE;
    keyboardMap[GLFW_KEY_LEFT_CONTROL] = KeyboardKey::CTRL_LEFT;
    keyboardMap[GLFW_KEY_RIGHT_CONTROL] = KeyboardKey::CTRL_RIGHT;
    keyboardMap[GLFW_KEY_LEFT_ALT] = KeyboardKey::ALT_LEFT;
    keyboardMap[GLFW_KEY_RIGHT_ALT] = KeyboardKey::ALT_RIGHT;
    keyboardMap[GLFW_KEY_LEFT_SHIFT] = KeyboardKey::SHIFT_LEFT;
    keyboardMap[GLFW_KEY_RIGHT_SHIFT] = KeyboardKey::SHIFT_RIGHT;
    keyboardMap[GLFW_KEY_LEFT] = KeyboardKey::ARROW_LEFT;
    keyboardMap[GLFW_KEY_RIGHT] = KeyboardKey::ARROW_RIGHT;
    keyboardMap[GLFW_KEY_UP] = KeyboardKey::ARROW_UP;
    keyboardMap[GLFW_KEY_DOWN] = KeyboardKey::ARROW_DOWN;
    keyboardMap[GLFW_KEY_PAGE_UP] = KeyboardKey::PAGE_UP;
    keyboardMap[GLFW_KEY_PAGE_DOWN] = KeyboardKey::PAGE_DOWN;
}

std::string Main::retrieveResourcesDirectory(char *argv[]) const {
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "resources/";
}

std::string Main::retrieveSaveDirectory(char *argv[]) const {
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "save/";
}

GLFWwindow* Main::createWindow(int argc, char *argv[]) {
    GLFWwindow *window;
    const char* windowTitle = "Urchin Engine Test";

    if (argumentsContains("--windowed", argc, argv)) {
        window = glfwCreateWindow(1200, 675, windowTitle, nullptr, nullptr);
    } else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, windowTitle, monitor, nullptr);
    }

    if (!window) {
        throw std::runtime_error("Impossible to create the GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); //vertical synchronization active

    return window;
}

void Main::charCallback(GLFWwindow* window, unsigned int codepoint) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main->windowController->isEventCallbackActive()) {
        main->charPressEvents.push_back(codepoint);
    }
}

void Main::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main->windowController->isEventCallbackActive()) {
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            main->keyPressEvents.push_back(key);
        } else if (action == GLFW_RELEASE) {
            main->onKeyReleased(key);
        }
    }
}

void Main::mouseKeyCallback(GLFWwindow* window, int button, int action, int) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main->windowController->isEventCallbackActive()) {
        if (action == GLFW_PRESS) {
            main->onMouseButtonPressed(button);
        } else if (action == GLFW_RELEASE) {
            main->onMouseButtonReleased(button);
        }
    }
}

void Main::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main->windowController->isEventCallbackActive()) {
        main->onMouseMove(static_cast<int>(x), static_cast<int>(y));
    }
}

void Main::windowSizeCallback(GLFWwindow* window, int width, int height) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main->windowController->isEventCallbackActive()) {
        main->mainDisplayer->resize(width, height);
    }
}

void Main::handleInputEvents() {
    propagatePressKeyEvent = true;
    propagateReleaseKeyEvent = true;
    glfwPollEvents();

    if (!charPressEvents.empty()) {
        for (unsigned int charUnicode : charPressEvents) {
            onChar(charUnicode);
        }
        charPressEvents.clear();
    }

    if (!keyPressEvents.empty()) {
        for (int keyPress : keyPressEvents) {
            onKeyPressed(keyPress);
        }
        keyPressEvents.clear();
    }
}

void Main::onChar(unsigned int unicode) {
    //engine
    if (propagatePressKeyEvent && unicode < 256) {
        propagatePressKeyEvent = mainDisplayer->getSceneManager()->onChar(unicode);
    }
}

void Main::onKeyPressed(int key) {
    //engine
    if (propagatePressKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::DELETE);
        }
    }

    //game
    if (propagatePressKeyEvent) {
        mainDisplayer->onKeyPressed(toKeyboardKey(key));
    }
}

void Main::onKeyReleased(int key) {
    //engine
    if (propagateReleaseKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::DELETE);
        }
    }

    //game
    if (propagateReleaseKeyEvent) {
        mainDisplayer->onKeyReleased(toKeyboardKey(key));
    }
}

KeyboardKey Main::toKeyboardKey(int key) {
    const char* charKey = glfwGetKeyName(key, 0);
    if(charKey && charKey[0] >= 'a' && charKey[0] <= 'z') {
        int keyShift = charKey[0] - 'a';
        return static_cast<KeyboardKey>(KeyboardKey::A + keyShift);
    }

    auto it = keyboardMap.find(key);
    if (it!=keyboardMap.end()) {
        return it->second;
    } else {
        return KeyboardKey::UNKNOWN_KEY;
    }
}

void Main::onMouseButtonPressed(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mainDisplayer->getSceneManager()->onKeyPress(urchin::InputDeviceKey::MOUSE_RIGHT);
    }
}

void Main::onMouseButtonReleased(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        mainDisplayer->getSceneManager()->onKeyRelease(urchin::InputDeviceKey::MOUSE_RIGHT);
    }
}

void Main::onMouseMove(int x, int y) {
    //engine
    if (x!=0 || y!=0) {
        mainDisplayer->onMouseMove(x, y);
    }
}

bool Main::argumentsContains(const std::string& argName, int argc, char *argv[]) const {
    for (int i=1;i<argc;++i) {
        if (std::string(argv[i]).find(argName)!=std::string::npos) {
            return true;
        }
    }
    return false;
}

void Main::clearResources(GLFWwindow *&window, WindowController *&windowController) {
    delete mainDisplayer;
    mainDisplayer = nullptr;

    delete windowController;
    windowController = nullptr;

    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

void Main::failureExit(GLFWwindow *&window, WindowController *&windowController) {
    std::string logFilename = dynamic_cast<urchin::FileLogger&>(urchin::Logger::logger()).getFilename();
    std::cerr<<"Application stopped with issue (log: "<<logFilename<<")"<<std::endl;

    clearResources(window, windowController);

    std::exit(1);
}
