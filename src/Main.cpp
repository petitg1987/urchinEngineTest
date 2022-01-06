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

        FileSystem::instance().setupResourcesDirectory(retrieveResourcesDirectory(argv));
        ConfigService::instance().loadProperties("engine.properties");
        UISkinService::instance().setSkin("ui/skinDefinition.uda");

        window = createWindow(isWindowed);

        glfwSetWindowUserPointer(window, (void*)this);
        glfwSetCharCallback(window, charCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseKeyCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        context = createMainContext(window, isDebugModeOn);
        game = std::make_unique<Game>(*context);
        context->getWindowController().cleanEvents(); //ignore events occurred during initialization phase

        while (!glfwWindowShouldClose(window)) {
            handleInputEvents();

            game->refresh();
            context->getScene().display();
        }

        if (Logger::instance().hasFailure()) {
            crashReporter->onLogContainFailure();
            clearResources(window);
            _exit(1);
        } else {
            clearResources(window);
            if (Logger::instance().hasFailure()) {
                _exit(1);
            }
            Logger::instance().purge();
        }
    } catch (const std::exception& e) {
        crashReporter->onException(e);
        clearResources(window);
        _exit(1);
    }
}

std::unique_ptr<MainContext> Main::createMainContext(GLFWwindow* window, bool isDebugModeOn) const {
    auto windowController = std::make_unique<WindowController>(window, isDebugModeOn);

    auto scene = std::make_unique<Scene>(WindowController::windowRequiredExtensions(), windowController->newSurfaceCreator(), windowController->newFramebufferSizeRetriever());
    scene->updateVerticalSync(false);

    auto soundEnvironment = std::make_unique<SoundEnvironment>();

    return std::make_unique<MainContext>(std::move(scene), std::move(windowController), std::move(soundEnvironment));
}

void Main::glfwErrorCallback(int error, const char* description) {
    std::string errorDescription = (description != nullptr) ? std::string(description) : "";
    if (error == GLFW_INVALID_VALUE && errorDescription.find("Invalid scancode") != std::string::npos) {
        //see https://github.com/glfw/glfw/issues/1785 (also happens on some Windows laptops)
        return; //full message: "Invalid scancode 0"
    } else if (error == GLFW_PLATFORM_ERROR && errorDescription.find("Iconification") != std::string::npos) {
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

GLFWwindow* Main::createWindow(bool isWindowed) {
    GLFWwindow *window;
    const char* windowTitle = "Urchin Engine Test";

    if (isWindowed) {
        window = glfwCreateWindow(2560, 1440, windowTitle, nullptr, nullptr);
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
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main && main->context->getWindowController().isEventCallbackActive()) {
        main->charEvents.push_back(unicodeCharacter);
    }
}

void Main::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main && main->context->getWindowController().isEventCallbackActive()) {
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            if (key == GLFW_KEY_ESCAPE) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            main->keyEvents.emplace_back(key, true /* pressed */);
        } else if (action == GLFW_RELEASE) {
            main->keyEvents.emplace_back(key, false /* released */);
        }
    }
}

void Main::mouseKeyCallback(GLFWwindow* window, int button, int action, int) {
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main && main->context->getWindowController().isEventCallbackActive()) {
        if (action == GLFW_PRESS) {
            main->onMouseButtonPressed(button);
        } else if (action == GLFW_RELEASE) {
            main->onMouseButtonReleased(button);
        }
    }
}

void Main::cursorPositionCallback(GLFWwindow* window, double x, double y) {
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main && main->context->getWindowController().isEventCallbackActive()) {
        main->onMouseMove((int)x, (int)y);
    }
}

void Main::scrollCallback(GLFWwindow* window, double, double offsetY) {
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main && main->context->getWindowController().isEventCallbackActive()) {
        main->onScroll(offsetY);
    }
}

void Main::framebufferSizeCallback(GLFWwindow* window, int, int) {
    auto main = static_cast<Main*>(glfwGetWindowUserPointer(window));
    if (main) {
        //engine
        main->context->getScene().onResize();
    }
}

void Main::handleInputEvents() {
    propagatePressKeyEvent = true;
    propagateReleaseKeyEvent = true;
    glfwPollEvents();

    for (unsigned int charUnicode : charEvents) {
        onChar(charUnicode);
    }
    charEvents.clear();

    for (auto [key, isKeyPressed] : keyEvents) {
        if (isKeyPressed) {
            onKeyPressed(key);
        } else {
            onKeyReleased(key);
        }
    }
    keyEvents.clear();
}

void Main::onChar(char32_t unicodeCharacter) {
    //engine
    if (propagatePressKeyEvent) {
        propagatePressKeyEvent = context->getScene().onChar(unicodeCharacter);
    }
}

void Main::onKeyPressed(int key) {
    //engine
    if (propagatePressKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::DELETE_KEY);
        }
    }

    //game
    if (propagatePressKeyEvent) {
        game->onKeyPressed(toInputKey(key));
    }
}

void Main::onKeyReleased(int key) {
    //engine
    if (propagateReleaseKeyEvent) {
        if (key == GLFW_KEY_LEFT) {
            propagatePressKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::LEFT_ARROW);
        } else if (key == GLFW_KEY_RIGHT) {
            propagatePressKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::RIGHT_ARROW);
        } else if (key == GLFW_KEY_BACKSPACE) {
            propagatePressKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::BACKSPACE);
        } else if (key == GLFW_KEY_DELETE) {
            propagatePressKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::DELETE_KEY);
        }
    }

    //game
    if (propagateReleaseKeyEvent) {
        game->onKeyReleased(toInputKey(key));
    }
}

void Main::onMouseButtonPressed(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        propagateReleaseKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        propagateReleaseKeyEvent = context->getScene().onKeyPress((unsigned int)InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagatePressKeyEvent) {
        game->onKeyPressed(toInputKey(button));
    }
}

void Main::onMouseButtonReleased(int button) {
    //engine
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        propagateReleaseKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::MOUSE_LEFT);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        propagateReleaseKeyEvent = context->getScene().onKeyRelease((unsigned int)InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagateReleaseKeyEvent) {
        game->onKeyReleased(toInputKey(button));
    }
}

void Main::onMouseMove(double x, double y) const {
    if (x != 0 || y != 0) {
        //engine
        context->getScene().onMouseMove(x, y);
    }
}

void Main::onScroll(double offsetY) const {
    if (offsetY != 0) {
        //engine
        context->getScene().onScroll(offsetY);
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

bool Main::argumentsContains(const std::string& argName, int argc, char *argv[]) const {
    for (int i = 1; i < argc;++i) {
        if (std::string(argv[i]).find(argName) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void Main::clearResources(GLFWwindow*& window) {
    game.reset(nullptr);
    context.reset(nullptr);
    SingletonContainer::destroyAllSingletons();

    if (window != nullptr) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}
