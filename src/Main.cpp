#include <unistd.h>

#include <Main.h>
using namespace urchin;

int main(int argc, char *argv[]) {
    Main main;
    main.execute(argc, argv);

    return 0;
}

Main::Main() :
        propagatePressKeyEvent(true),
        propagateReleaseKeyEvent(true) {
    crashReporter = std::make_shared<CrashReporter>();
}

void Main::execute(int argc, char *argv[]) {
    initializeInputKeyMap();

    Logger::setupCustomInstance(std::make_unique<FileLogger>("urchinEngineTest.log"));
    SignalHandler::instance().registerSignalReceptor(crashReporter);

    Logger::instance().logInfo("Application started");
    GLFWwindow* window = nullptr;

    bool isWindowed = argumentsContains("--windowed", argc, argv);
    bool isDebugModeOn = argumentsContains("--debug", argc, argv);

    try {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit()) {
            throw std::runtime_error("Impossible to initialize GLFW library");
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = createWindow(isWindowed);
        windowController = std::make_unique<WindowController>(window, isDebugModeOn);
        std::string resourcesDirectory = retrieveResourcesDirectory(argv);
        std::string saveDirectory = retrieveSaveDirectory(argv);

        mainDisplayer = std::make_unique<MainDisplayer>(*windowController);
        mainDisplayer->initialize(resourcesDirectory);

        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetCharCallback(window, charCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseKeyCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        windowController->cleanEvents(); //ignore events occurred during initialization phase

        while (!glfwWindowShouldClose(window)) {
            handleInputEvents();

            mainDisplayer->paint();
        }

        if (Logger::instance().hasFailure()) {
            crashReporter->onLogContainFailure();
            clearResources(window);
            _exit(1);
        } else {
            clearResources(window);
            Logger::instance().purge();
        }
    } catch (std::exception& e) {
        crashReporter->onException(e);
        clearResources(window);
        _exit(1);
    }
}

void Main::glfwErrorCallback(int error, const char* description) {
    if (error == GLFW_INVALID_VALUE && strcmp(description, "Invalid scancode") == 0) {
        return; //see https://github.com/glfw/glfw/issues/1785 (also happens on some Windows laptops)
    } else if (error == GLFW_PLATFORM_ERROR && std::string(description).find("Iconification") != std::string::npos) {
        Logger::instance().logInfo("Window iconification GLFW error ignored: " + std::string(description));
        return; //full message: "X11: Iconification of full screen windows requires a WM that supports EWMH full screen"
    }
    Logger::instance().logWarning("GLFW error (code: " + std::to_string(error) + "): " + description);
}

void Main::initializeInputKeyMap() {
    //keyboard
    for (int i = 0; i < 10; ++i) {
        inputKeyMap[GLFW_KEY_0 + i] = static_cast<Control::Key>(Control::Key::K0 + i);
    }
    for (int i = 0; i < 12; ++i) {
        inputKeyMap[GLFW_KEY_F1 + i] = static_cast<Control::Key>(Control::Key::F1 + i);
    }
    for (int i = 0; i < 10; ++i) {
        inputKeyMap[GLFW_KEY_KP_0 + i] = static_cast<Control::Key>(Control::Key::NUM_PAD_0 + i);
    }
    inputKeyMap[GLFW_KEY_KP_DECIMAL] = Control::Key::NUM_PAD_DECIMAL;
    inputKeyMap[GLFW_KEY_KP_DIVIDE] = Control::Key::NUM_PAD_DIVIDE;
    inputKeyMap[GLFW_KEY_KP_MULTIPLY] = Control::Key::NUM_PAD_MULTIPLY;
    inputKeyMap[GLFW_KEY_KP_SUBTRACT] = Control::Key::NUM_PAD_SUBTRACT;
    inputKeyMap[GLFW_KEY_KP_ADD] = Control::Key::NUM_PAD_ADD;
    inputKeyMap[GLFW_KEY_KP_ENTER] = Control::Key::NUM_PAD_ENTER;
    inputKeyMap[GLFW_KEY_KP_EQUAL] = Control::Key::NUM_PAD_EQUAL;
    inputKeyMap[GLFW_KEY_ESCAPE] = Control::Key::ESCAPE;
    inputKeyMap[GLFW_KEY_SPACE] = Control::Key::SPACE;
    inputKeyMap[GLFW_KEY_LEFT_CONTROL] = Control::Key::CTRL_LEFT;
    inputKeyMap[GLFW_KEY_RIGHT_CONTROL] = Control::Key::CTRL_RIGHT;
    inputKeyMap[GLFW_KEY_LEFT_ALT] = Control::Key::ALT_LEFT;
    inputKeyMap[GLFW_KEY_RIGHT_ALT] = Control::Key::ALT_RIGHT;
    inputKeyMap[GLFW_KEY_LEFT_SHIFT] = Control::Key::SHIFT_LEFT;
    inputKeyMap[GLFW_KEY_RIGHT_SHIFT] = Control::Key::SHIFT_RIGHT;
    inputKeyMap[GLFW_KEY_LEFT_SUPER] = Control::Key::SUPER_LEFT;
    inputKeyMap[GLFW_KEY_RIGHT_SUPER] = Control::Key::SUPER_RIGHT;
    inputKeyMap[GLFW_KEY_MENU] = Control::Key::MENU;
    inputKeyMap[GLFW_KEY_LEFT] = Control::Key::ARROW_LEFT;
    inputKeyMap[GLFW_KEY_RIGHT] = Control::Key::ARROW_RIGHT;
    inputKeyMap[GLFW_KEY_UP] = Control::Key::ARROW_UP;
    inputKeyMap[GLFW_KEY_DOWN] = Control::Key::ARROW_DOWN;
    inputKeyMap[GLFW_KEY_ENTER] = Control::Key::ENTER;
    inputKeyMap[GLFW_KEY_TAB] = Control::Key::TAB;
    inputKeyMap[GLFW_KEY_BACKSPACE] = Control::Key::BACKSPACE;
    inputKeyMap[GLFW_KEY_INSERT] = Control::Key::INSERT;
    inputKeyMap[GLFW_KEY_DELETE] = Control::Key::DEL;
    inputKeyMap[GLFW_KEY_HOME] = Control::Key::HOME;
    inputKeyMap[GLFW_KEY_END] = Control::Key::END;
    inputKeyMap[GLFW_KEY_PAGE_UP] = Control::Key::PAGE_UP;
    inputKeyMap[GLFW_KEY_PAGE_DOWN] = Control::Key::PAGE_DOWN;
    inputKeyMap[GLFW_KEY_CAPS_LOCK] = Control::Key::CAPS_LOCK;
    inputKeyMap[GLFW_KEY_SCROLL_LOCK] = Control::Key::SCROLL_LOCK;
    inputKeyMap[GLFW_KEY_NUM_LOCK] = Control::Key::NUM_LOCK;
    inputKeyMap[GLFW_KEY_PRINT_SCREEN] = Control::Key::PRINT_SCREEN;
    inputKeyMap[GLFW_KEY_PAUSE] = Control::Key::PAUSE;

    //mouse
    inputKeyMap[GLFW_MOUSE_BUTTON_LEFT] = Control::Key::LMB;
    inputKeyMap[GLFW_MOUSE_BUTTON_RIGHT] = Control::Key::RMB;
    inputKeyMap[GLFW_MOUSE_BUTTON_MIDDLE] = Control::Key::MMB;
    inputKeyMap[GLFW_MOUSE_BUTTON_4] = Control::Key::MOUSE_F1;
    inputKeyMap[GLFW_MOUSE_BUTTON_5] = Control::Key::MOUSE_F2;
    inputKeyMap[GLFW_MOUSE_BUTTON_6] = Control::Key::MOUSE_F3;
    inputKeyMap[GLFW_MOUSE_BUTTON_7] = Control::Key::MOUSE_F4;
    inputKeyMap[GLFW_MOUSE_BUTTON_8] = Control::Key::MOUSE_F5;
}

std::string Main::retrieveResourcesDirectory(char *argv[]) {
    return FileUtil::getDirectory(std::string(argv[0])) + "resources/";
}

std::string Main::retrieveSaveDirectory(char *argv[]) {
    return FileUtil::getDirectory(std::string(argv[0])) + "save/";
}

GLFWwindow* Main::createWindow(bool isWindowed) {
    GLFWwindow *window;
    const char* windowTitle = "Urchin Engine Test";

    if (isWindowed) {
        window = glfwCreateWindow(1200, 675, windowTitle, nullptr, nullptr);
    } else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, windowTitle, monitor, nullptr);
    }

    if (!window) {
        throw std::runtime_error("Impossible to create the GLFW window");
    }

    return window;
}

void Main::charCallback(GLFWwindow* window, unsigned int unicodeCharacter) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main && main->windowController->isEventCallbackActive()) {
        main->charPressEvents.push_back(unicodeCharacter);
    }
}

void Main::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main && main->windowController->isEventCallbackActive()) {
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
    if (main && main->windowController->isEventCallbackActive()) {
        if (action == GLFW_PRESS) {
            main->onMouseButtonPressed(button);
        } else if (action == GLFW_RELEASE) {
            main->onMouseButtonReleased(button);
        }
    }
}

void Main::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main && main->windowController->isEventCallbackActive()) {
        main->onMouseMove((int)x, (int)y);
    }
}

void Main::framebufferSizeCallback(GLFWwindow* window, int, int) {
    Main* main = (Main*)glfwGetWindowUserPointer(window);
    if (main) {
        main->mainDisplayer->resize();
    }
}

void Main::handleInputEvents() {
    propagatePressKeyEvent = true;
    propagateReleaseKeyEvent = true;
    glfwPollEvents();

    if (!charPressEvents.empty()) {
        for (char32_t unicodeCharacter : charPressEvents) {
            onChar(unicodeCharacter);
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

void Main::onChar(char32_t unicodeCharacter) {
    //engine
    if (propagatePressKeyEvent) {
        propagatePressKeyEvent = mainDisplayer->getScene()->onChar(unicodeCharacter);
    }
}

void Main::onKeyPressed(int key) {
    //engine
    if (propagatePressKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::DELETE_KEY);
        }
    }

    //game
    if (propagatePressKeyEvent) {
        mainDisplayer->onKeyPressed(toInputKey(key));
    }
}

void Main::onKeyReleased(int key) {
    //engine
    if (propagateReleaseKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::DELETE_KEY);
        }
    }

    //game
    if (propagateReleaseKeyEvent) {
        mainDisplayer->onKeyReleased(toInputKey(key));
    }
}

void Main::onMouseButtonPressed(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        propagateReleaseKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        propagateReleaseKeyEvent = mainDisplayer->getScene()->onKeyPress(InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagatePressKeyEvent) {
        mainDisplayer->onKeyPressed(toInputKey(button));
    }
}

void Main::onMouseButtonReleased(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        propagateReleaseKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        propagateReleaseKeyEvent = mainDisplayer->getScene()->onKeyRelease(InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagateReleaseKeyEvent) {
        mainDisplayer->onKeyReleased(toInputKey(button));
    }
}

void Main::onMouseMove(double x, double y) {
    //engine
    if (x != 0 || y != 0) {
        mainDisplayer->onMouseMove(x, y);
    }
}

Control::Key Main::toInputKey(int key) {
    auto itFind = inputKeyMap.find(key);
    if (itFind != inputKeyMap.end()) {
        return itFind->second;
    }

    const char* charKey = glfwGetKeyName(key, 0);
    if (charKey) {
        if (charKey[0] >= 'a' && charKey[0] <= 'z') {
            int keyShift = charKey[0] - 'a';
            return static_cast<Control::Key>(Control::Key::A + keyShift);
        } else if (charKey[0] == '=') {
            return Control::Key::EQUAL;
        } else if (charKey[0] == ':') {
            return Control::Key::COLON;
        } else if (charKey[0] == ';') {
            return Control::Key::SEMICOLON;
        } else if (charKey[0] == ',') {
            return Control::Key::COMMA;
        } else if (charKey[0] == '-') {
            return Control::Key::MINUS;
        } else if (charKey[0] == '/') { //for QWERTY keyboards
            return Control::Key::SLASH;
        } else if (charKey[0] == '\\') { //for QWERTY keyboards
            return Control::Key::BACKSLASH;
        } else if (charKey[0] == '\'') { //for QWERTY keyboards
            return Control::Key::APOSTROPHE;
        } else if (charKey[0] == '[') { //for QWERTY keyboards
            return Control::Key::LEFT_BRACKET;
        } else if (charKey[0] == ']') { //for QWERTY keyboards
            return Control::Key::RIGHT_BRACKET;
        } else if (charKey[0] == '.') { //for QWERTY keyboards
            return Control::Key::PERIOD;
        } else if (charKey[0] == '`') { //for QWERTY keyboards
            return Control::Key::GRAVE_ACCENT;
        }
    }

    return Control::Key::UNKNOWN_KEY;
}

bool Main::argumentsContains(const std::string& argName, int argc, char *argv[]) {
    for (int i = 1; i < argc;++i) {
        if (std::string(argv[i]).find(argName) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void Main::clearResources(GLFWwindow*& window) {
    mainDisplayer.reset(nullptr);

    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
