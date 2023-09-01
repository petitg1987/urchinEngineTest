#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <Main.h>
using namespace urchin;

int main(int argc, char* argv[]) {
    Main main;
    main.execute(std::span<char*>{argv, (std::size_t)argc});

    return 0;
}

Main::Main() :
        propagatePressKeyEvent(true),
        propagateReleaseKeyEvent(true),
        altLeftKeyPressed(false) {
    crashReporter = std::make_shared<CrashReporter>();
}

void Main::execute(std::span<char*> args) {
    initializeInputKeyMap();

    Logger::setupCustomInstance(std::make_unique<FileLogger>("urchinEngineTest.log"));
    SignalHandler::instance().registerSignalReceptor(crashReporter);

    Logger::instance().logInfo("Application started");
    SDL_Window* window = nullptr;

    bool isWindowed = argumentsContains("--windowed", args);
    bool isDevModeOn = argumentsContains("--dev", args);

    try {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error("Impossible to initialize SDL library: " + std::string(SDL_GetError()));
        }
        if (SDL_Vulkan_LoadLibrary(nullptr) != 0) {
            throw std::runtime_error("Impossible to load SDL Vulkan library: " + std::string(SDL_GetError()));
        }

        FileSystem::instance().setupResourcesDirectory(retrieveResourcesDirectory(args[0]));
        ConfigService::instance().loadProperties("engine.properties");
        UISkinService::instance().setSkin("ui/skinDefinition.uda");

        window = createWindow(isWindowed);
        context = createMainContext(window, isDevModeOn);
        game = std::make_unique<Game>(*context);
        context->getWindowController().cleanEvents(); //ignore events occurred during initialization phase

        while (!context->isExitRequired()) {
            handleInputEvents(window);

            game->refresh();
            context->getScene().display();
        }

        if ( (isDevModeOn && Logger::instance().hasWarningOrError()) || (!isDevModeOn && Logger::instance().hasError()) ) {
            crashReporter->onLogContainFailure();
            clearResources(window);
            _exit(1);
        } else {
            clearResources(window);
            if (Logger::instance().hasWarningOrError()) {
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

std::unique_ptr<MainContext> Main::createMainContext(SDL_Window* window, bool isDevModeOn) const {
    auto windowController = std::make_unique<WindowController>(window, isDevModeOn);
    auto scene = std::make_unique<Scene>(windowController->windowRequiredExtensions(), windowController->newSurfaceCreator(), windowController->newFramebufferSizeRetriever(), false);
    auto soundEnvironment = std::make_unique<SoundEnvironment>();

    return std::make_unique<MainContext>(std::move(scene), std::move(windowController), std::move(soundEnvironment));
}

void Main::initializeInputKeyMap() {
    //keyboard
    for (int i = 0; i < 10; ++i) {
        inputKeyMap[SDLK_0 + i] = static_cast<Control::Key>(Control::Key::K0 + i);
    }
    inputKeyMap[SDLK_F1] = Control::Key::F1;
    inputKeyMap[SDLK_F2] = Control::Key::F2;
    inputKeyMap[SDLK_F3] = Control::Key::F3;
    inputKeyMap[SDLK_F4] = Control::Key::F4;
    inputKeyMap[SDLK_F5] = Control::Key::F5;
    inputKeyMap[SDLK_F6] = Control::Key::F6;
    inputKeyMap[SDLK_F7] = Control::Key::F7;
    inputKeyMap[SDLK_F8] = Control::Key::F8;
    inputKeyMap[SDLK_F9] = Control::Key::F9;
    inputKeyMap[SDLK_F10] = Control::Key::F10;
    inputKeyMap[SDLK_F11] = Control::Key::F11;
    inputKeyMap[SDLK_F12] = Control::Key::F12;
    inputKeyMap[SDLK_KP_0] = Control::Key::NUM_PAD_0;
    inputKeyMap[SDLK_KP_1] = Control::Key::NUM_PAD_1;
    inputKeyMap[SDLK_KP_2] = Control::Key::NUM_PAD_2;
    inputKeyMap[SDLK_KP_3] = Control::Key::NUM_PAD_3;
    inputKeyMap[SDLK_KP_4] = Control::Key::NUM_PAD_4;
    inputKeyMap[SDLK_KP_5] = Control::Key::NUM_PAD_5;
    inputKeyMap[SDLK_KP_6] = Control::Key::NUM_PAD_6;
    inputKeyMap[SDLK_KP_7] = Control::Key::NUM_PAD_7;
    inputKeyMap[SDLK_KP_8] = Control::Key::NUM_PAD_8;
    inputKeyMap[SDLK_KP_9] = Control::Key::NUM_PAD_9;
    inputKeyMap[SDLK_KP_DECIMAL] = Control::Key::NUM_PAD_DECIMAL;
    inputKeyMap[SDLK_KP_DIVIDE] = Control::Key::NUM_PAD_DIVIDE;
    inputKeyMap[SDLK_KP_MULTIPLY] = Control::Key::NUM_PAD_MULTIPLY;
    inputKeyMap[SDLK_KP_MINUS] = Control::Key::NUM_PAD_SUBTRACT;
    inputKeyMap[SDLK_KP_PLUS] = Control::Key::NUM_PAD_ADD;
    inputKeyMap[SDLK_KP_ENTER] = Control::Key::NUM_PAD_ENTER;
    inputKeyMap[SDLK_KP_EQUALS] = Control::Key::NUM_PAD_EQUAL;
    inputKeyMap[SDLK_ESCAPE] = Control::Key::ESCAPE;
    inputKeyMap[SDLK_SPACE] = Control::Key::SPACE;
    inputKeyMap[SDLK_LCTRL] = Control::Key::CTRL_LEFT;
    inputKeyMap[SDLK_RCTRL] = Control::Key::CTRL_RIGHT;
    inputKeyMap[SDLK_LALT] = Control::Key::ALT_LEFT;
    inputKeyMap[SDLK_RALT] = Control::Key::ALT_RIGHT;
    inputKeyMap[SDLK_LSHIFT] = Control::Key::SHIFT_LEFT;
    inputKeyMap[SDLK_RSHIFT] = Control::Key::SHIFT_RIGHT;
    inputKeyMap[SDLK_LGUI] = Control::Key::SUPER_LEFT;
    inputKeyMap[SDLK_RGUI] = Control::Key::SUPER_RIGHT;
    inputKeyMap[SDLK_MENU] = Control::Key::MENU;
    inputKeyMap[SDLK_LEFT] = Control::Key::ARROW_LEFT;
    inputKeyMap[SDLK_RIGHT] = Control::Key::ARROW_RIGHT;
    inputKeyMap[SDLK_UP] = Control::Key::ARROW_UP;
    inputKeyMap[SDLK_DOWN] = Control::Key::ARROW_DOWN;
    inputKeyMap[SDLK_RETURN] = Control::Key::ENTER;
    inputKeyMap[SDLK_TAB] = Control::Key::TAB;
    inputKeyMap[SDLK_BACKSPACE] = Control::Key::BACKSPACE;
    inputKeyMap[SDLK_INSERT] = Control::Key::INSERT;
    inputKeyMap[SDLK_DELETE] = Control::Key::DEL;
    inputKeyMap[SDLK_HOME] = Control::Key::HOME;
    inputKeyMap[SDLK_END] = Control::Key::END;
    inputKeyMap[SDLK_PAGEUP] = Control::Key::PAGE_UP;
    inputKeyMap[SDLK_PAGEDOWN] = Control::Key::PAGE_DOWN;
    inputKeyMap[SDLK_CAPSLOCK] = Control::Key::CAPS_LOCK;
    inputKeyMap[SDLK_SCROLLLOCK] = Control::Key::SCROLL_LOCK;
    inputKeyMap[SDLK_NUMLOCKCLEAR] = Control::Key::NUM_LOCK;
    inputKeyMap[SDLK_PRINTSCREEN] = Control::Key::PRINT_SCREEN;
    inputKeyMap[SDLK_PAUSE] = Control::Key::PAUSE;

    //mouse
    inputKeyMap[SDL_BUTTON_LEFT] = Control::Key::LMB;
    inputKeyMap[SDL_BUTTON_RIGHT] = Control::Key::RMB;
    inputKeyMap[SDL_BUTTON_MIDDLE] = Control::Key::MMB;
    inputKeyMap[SDL_BUTTON_X1] = Control::Key::MOUSE_F1;
    inputKeyMap[SDL_BUTTON_X2] = Control::Key::MOUSE_F2;
}

std::string Main::retrieveResourcesDirectory(const char* firstArg) {
    return FileUtil::getDirectory(std::string_view(firstArg)) + "resources/";
}

SDL_Window* Main::createWindow(bool isWindowed) {
    Point2<int> windowSize;
    Uint32 flags;
    if (isWindowed) {
        windowSize = WindowController::optimumWindowSize();
        flags = SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE;
    } else {
        SDL_DisplayMode desktopDisplayMode;
        if (SDL_GetDesktopDisplayMode(0, &desktopDisplayMode) != 0) {
            throw std::runtime_error("Impossible to get SDL current display mode: " + std::string(SDL_GetError()));
        }
        windowSize = Point2<int>(desktopDisplayMode.w, desktopDisplayMode.h);
        flags = SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_FULLSCREEN;
    }

    SDL_Window* window = SDL_CreateWindow("Urchin Engine Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.X, windowSize.Y, flags);
    if (!window) {
        throw std::runtime_error("Impossible to create the SDL window");
    }

    return window;
}

void Main::handleInputEvents(SDL_Window* window) {
    propagatePressKeyEvent = true;
    propagateReleaseKeyEvent = true;

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        events.emplace_back(e);
    }
    for (std::size_t i = 0; i < events.size(); ++i) {
        if (events[i].type == SDL_KEYDOWN && i + 1 < events.size() && events[i + 1].type == SDL_TEXTINPUT) {
            std::swap(events[i], events[i + 1]); //text input must be treated before key down to handle correctly the event propagation flag
        }

        //handle Alt+Enter to switch between windowed/fullscreen mode
        if (events[i].type == SDL_KEYDOWN) {
            if (events[i].key.keysym.sym == SDLK_LALT) {
                altLeftKeyPressed = true;
            } else if (altLeftKeyPressed && events[i].key.keysym.sym == SDLK_RETURN) {
                bool isFullScreen = (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
                context->getWindowController().updateWindowedMode(isFullScreen);
                context->getScene().onResize(); //manually resize because the windowed mode change does not always trigger the resize
                continue;
            }
        } else if (events[i].type == SDL_KEYUP) {
            if (events[i].key.keysym.sym == SDLK_LALT) {
                altLeftKeyPressed = false;
            }
        }

        if (events[i].type == SDL_MOUSEMOTION) {
            onMouseMove(events[i].motion.x, events[i].motion.y, events[i].motion.xrel, events[i].motion.yrel);
        } else if (events[i].type == SDL_MOUSEBUTTONDOWN) {
            onMouseButtonPressed(events[i].button.button);
        } else if (events[i].type == SDL_MOUSEBUTTONUP) {
            onMouseButtonReleased(events[i].button.button);
        } else if (events[i].type == SDL_MOUSEWHEEL) {
            onScroll(events[i].wheel.preciseY);
        } else if (events[i].type == SDL_KEYDOWN) {
            onKeyPressed(events[i].key.keysym.sym, events[i].key.repeat != 0);
        } else if (events[i].type == SDL_KEYUP) {
            onKeyReleased(events[i].key.keysym.sym);
        } else if (events[i].type == SDL_TEXTINPUT) {
            static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::u32string u32text = converter.from_bytes(events[i].text.text);
            for (char32_t unicode: u32text) {
                onChar(unicode);
            }
        } else if (events[i].type == SDL_WINDOWEVENT) {
            if (events[i].window.event == SDL_WINDOWEVENT_RESIZED || events[i].window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                context->getScene().onResize();
            }
        } else if (events[i].type == SDL_QUIT) {
            context->exit();
        }
    }
    events.clear();
}

void Main::onChar(char32_t unicodeCharacter) {
    //engine
    if (propagatePressKeyEvent) {
        propagatePressKeyEvent = context->getScene().onChar(unicodeCharacter);
    }
}

void Main::onKeyPressed(int keyCode, bool isRepeatPress) {
    //engine
    if (propagatePressKeyEvent) {
        if (keyCode == SDLK_a) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::A);
        } else if (keyCode == SDLK_c) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::C);
        } else if (keyCode == SDLK_v) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::V);
        } else if (keyCode == SDLK_x) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::X);
        } else if (keyCode == SDLK_LCTRL || keyCode == SDLK_RCTRL) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::CTRL);
        } else if (keyCode == SDLK_LEFT) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::LEFT_ARROW);
        } else if (keyCode == SDLK_RIGHT) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::RIGHT_ARROW);
        } else if (keyCode == SDLK_UP) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::UP_ARROW);
        } else if (keyCode == SDLK_DOWN) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::DOWN_ARROW);
        } else if (keyCode == SDLK_BACKSPACE) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::BACKSPACE);
        } else if (keyCode == SDLK_DELETE) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::DELETE_KEY);
        } else if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
            propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::ENTER);
        }
    }

    //game
    if (propagatePressKeyEvent && !isRepeatPress) {
        if (keyCode == SDLK_ESCAPE) {
            context->exit();
        }
        game->onKeyPressed(toInputKey(keyCode));
    }
}

void Main::onKeyReleased(int keyCode) {
    //engine
    if (propagateReleaseKeyEvent) {
        if (keyCode == SDLK_a) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::A);
        } else if (keyCode == SDLK_c) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::C);
        } else if (keyCode == SDLK_v) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::V);
        } else if (keyCode == SDLK_x) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::X);
        } else if (keyCode == SDLK_LCTRL || keyCode == SDLK_RCTRL) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::CTRL);
        } else if (keyCode == SDLK_LEFT) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::LEFT_ARROW);
        } else if (keyCode == SDLK_RIGHT) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::RIGHT_ARROW);
        } else if (keyCode == SDLK_UP) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::UP_ARROW);
        } else if (keyCode == SDLK_DOWN) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::DOWN_ARROW);
        } else if (keyCode == SDLK_BACKSPACE) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::BACKSPACE);
        } else if (keyCode == SDLK_DELETE) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::DELETE_KEY);
        } else if (keyCode == SDLK_RETURN || keyCode == SDLK_KP_ENTER) {
            propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::ENTER);
        }
    }

    //game
    if (propagateReleaseKeyEvent) {
        game->onKeyReleased(toInputKey(keyCode));
    }
}

void Main::onMouseButtonPressed(int button) {
    //engine
    if (button == SDL_BUTTON_LEFT) {
        propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::MOUSE_LEFT);
    } else if (button == SDL_BUTTON_RIGHT) {
        propagatePressKeyEvent = context->getScene().onKeyPress(InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagatePressKeyEvent) {
        game->onKeyPressed(toInputKey(button));
    }
}

void Main::onMouseButtonReleased(int button) {
    //engine
    if (button == SDL_BUTTON_LEFT) {
        propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::MOUSE_LEFT);
    } else if (button == SDL_BUTTON_RIGHT) {
        propagateReleaseKeyEvent = context->getScene().onKeyRelease(InputDeviceKey::MOUSE_RIGHT);
    }

    //game
    if (propagateReleaseKeyEvent) {
        game->onKeyReleased(toInputKey(button));
    }
}

void Main::onMouseMove(double x, double y, double deltaX, double deltaY) const {
    if (x != 0 || y != 0) {
        //engine
        context->getScene().onMouseMove(x, y, deltaX, deltaY);
    }
}

void Main::onScroll(double offsetY) const {
    if (offsetY != 0) {
        //engine
        context->getScene().onScroll(offsetY);
    }
}

Control::Key Main::toInputKey(int keyCode) {
    auto itFind = inputKeyMap.find(keyCode);
    if (itFind != inputKeyMap.end()) {
        return itFind->second;
    }

    if (keyCode >= 'a' && keyCode <= 'z') {
        int keyShift = keyCode - 'a';
        return static_cast<Control::Key>(Control::Key::A + keyShift);
    } else if (keyCode == '=') {
        return Control::Key::EQUAL;
    } else if (keyCode == ':') {
        return Control::Key::COLON;
    } else if (keyCode == ';') {
        return Control::Key::SEMICOLON;
    } else if (keyCode == ',') {
        return Control::Key::COMMA;
    } else if (keyCode == '-') {
        return Control::Key::MINUS;
    } else if (keyCode == '/') { //for QWERTY keyboards
        return Control::Key::SLASH;
    } else if (keyCode == '\\') { //for QWERTY keyboards
        return Control::Key::BACKSLASH;
    } else if (keyCode == '\'') { //for QWERTY keyboards
        return Control::Key::APOSTROPHE;
    } else if (keyCode == '[') { //for QWERTY keyboards
        return Control::Key::LEFT_BRACKET;
    } else if (keyCode == ']') { //for QWERTY keyboards
        return Control::Key::RIGHT_BRACKET;
    } else if (keyCode == '.') { //for QWERTY keyboards
        return Control::Key::PERIOD;
    } else if (keyCode == '`') { //for QWERTY keyboards
        return Control::Key::GRAVE_ACCENT;
    }

    return Control::Key::UNKNOWN_KEY;
}

bool Main::argumentsContains(const std::string& argName, std::span<char*> args) const {
    return std::ranges::any_of(args, [&](const char* arg) { return std::string(arg).find(argName) != std::string::npos; });
}

void Main::clearResources(SDL_Window*& window) {
    game.reset(nullptr);
    context.reset(nullptr);
    SingletonContainer::destroyAllSingletons();

    SDL_Vulkan_UnloadLibrary();
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}
