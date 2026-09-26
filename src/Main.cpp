#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include "Main.h"
using namespace urchin;

int main(int argc, char* argv[]) {
    Main main;
    main.execute(std::span<char*>{argv, (std::size_t)argc});

    return 0;
}

Main::Main() :
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
        inputKeyMap[SDLK_0 + i] = static_cast<InputKey>(static_cast<int>(InputKey::K0) + i);
    }
    inputKeyMap[SDLK_F1] = InputKey::F1;
    inputKeyMap[SDLK_F2] = InputKey::F2;
    inputKeyMap[SDLK_F3] = InputKey::F3;
    inputKeyMap[SDLK_F4] = InputKey::F4;
    inputKeyMap[SDLK_F5] = InputKey::F5;
    inputKeyMap[SDLK_F6] = InputKey::F6;
    inputKeyMap[SDLK_F7] = InputKey::F7;
    inputKeyMap[SDLK_F8] = InputKey::F8;
    inputKeyMap[SDLK_F9] = InputKey::F9;
    inputKeyMap[SDLK_F10] = InputKey::F10;
    inputKeyMap[SDLK_F11] = InputKey::F11;
    inputKeyMap[SDLK_F12] = InputKey::F12;
    inputKeyMap[SDLK_KP_0] = InputKey::NUM_PAD_0;
    inputKeyMap[SDLK_KP_1] = InputKey::NUM_PAD_1;
    inputKeyMap[SDLK_KP_2] = InputKey::NUM_PAD_2;
    inputKeyMap[SDLK_KP_3] = InputKey::NUM_PAD_3;
    inputKeyMap[SDLK_KP_4] = InputKey::NUM_PAD_4;
    inputKeyMap[SDLK_KP_5] = InputKey::NUM_PAD_5;
    inputKeyMap[SDLK_KP_6] = InputKey::NUM_PAD_6;
    inputKeyMap[SDLK_KP_7] = InputKey::NUM_PAD_7;
    inputKeyMap[SDLK_KP_8] = InputKey::NUM_PAD_8;
    inputKeyMap[SDLK_KP_9] = InputKey::NUM_PAD_9;
    inputKeyMap[SDLK_KP_PERIOD] = InputKey::NUM_PAD_DECIMAL;
    inputKeyMap[SDLK_KP_DIVIDE] = InputKey::NUM_PAD_DIVIDE;
    inputKeyMap[SDLK_KP_MULTIPLY] = InputKey::NUM_PAD_MULTIPLY;
    inputKeyMap[SDLK_KP_MINUS] = InputKey::NUM_PAD_SUBTRACT;
    inputKeyMap[SDLK_KP_PLUS] = InputKey::NUM_PAD_ADD;
    inputKeyMap[SDLK_KP_ENTER] = InputKey::NUM_PAD_ENTER;
    inputKeyMap[SDLK_KP_EQUALS] = InputKey::NUM_PAD_EQUAL;
    inputKeyMap[SDLK_ESCAPE] = InputKey::ESCAPE;
    inputKeyMap[SDLK_SPACE] = InputKey::SPACE;
    inputKeyMap[SDLK_LCTRL] = InputKey::CTRL_LEFT;
    inputKeyMap[SDLK_RCTRL] = InputKey::CTRL_RIGHT;
    inputKeyMap[SDLK_LALT] = InputKey::ALT_LEFT;
    inputKeyMap[SDLK_RALT] = InputKey::ALT_RIGHT;
    inputKeyMap[SDLK_LSHIFT] = InputKey::SHIFT_LEFT;
    inputKeyMap[SDLK_RSHIFT] = InputKey::SHIFT_RIGHT;
    inputKeyMap[SDLK_LGUI] = InputKey::SUPER_LEFT;
    inputKeyMap[SDLK_RGUI] = InputKey::SUPER_RIGHT;
    inputKeyMap[SDLK_APPLICATION] = InputKey::MENU;
    inputKeyMap[SDLK_LEFT] = InputKey::ARROW_LEFT;
    inputKeyMap[SDLK_RIGHT] = InputKey::ARROW_RIGHT;
    inputKeyMap[SDLK_UP] = InputKey::ARROW_UP;
    inputKeyMap[SDLK_DOWN] = InputKey::ARROW_DOWN;
    inputKeyMap[SDLK_RETURN] = InputKey::ENTER;
    inputKeyMap[SDLK_TAB] = InputKey::TAB;
    inputKeyMap[SDLK_BACKSPACE] = InputKey::BACKSPACE;
    inputKeyMap[SDLK_INSERT] = InputKey::INSERT;
    inputKeyMap[SDLK_DELETE] = InputKey::DEL;
    inputKeyMap[SDLK_HOME] = InputKey::HOME;
    inputKeyMap[SDLK_END] = InputKey::END;
    inputKeyMap[SDLK_PAGEUP] = InputKey::PAGE_UP;
    inputKeyMap[SDLK_PAGEDOWN] = InputKey::PAGE_DOWN;
    inputKeyMap[SDLK_CAPSLOCK] = InputKey::CAPS_LOCK;
    inputKeyMap[SDLK_SCROLLLOCK] = InputKey::SCROLL_LOCK;
    inputKeyMap[SDLK_NUMLOCKCLEAR] = InputKey::NUM_LOCK;
    inputKeyMap[SDLK_PRINTSCREEN] = InputKey::PRINT_SCREEN;
    inputKeyMap[SDLK_PAUSE] = InputKey::PAUSE;

    //mouse
    inputKeyMap[SDL_BUTTON_LEFT] = InputKey::LMB;
    inputKeyMap[SDL_BUTTON_RIGHT] = InputKey::RMB;
    inputKeyMap[SDL_BUTTON_MIDDLE] = InputKey::MMB;
    inputKeyMap[SDL_BUTTON_X1] = InputKey::MOUSE_F1;
    inputKeyMap[SDL_BUTTON_X2] = InputKey::MOUSE_F2;
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
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        events.emplace_back(e);
    }
    bool keyPressConsumedByTextInput = false;
    for (std::size_t i = 0; i < events.size(); ++i) {
        if (events[i].type == SDL_KEYDOWN && i + 1 < events.size() && events[i + 1].type == SDL_TEXTINPUT) {
            std::swap(events[i], events[i + 1]); //text input must be treated before key down: when the text input is consumed, the key down is not propagated
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
            if (!keyPressConsumedByTextInput) {
                onKeyPressed(events[i].key.keysym.sym, events[i].key.repeat != 0);
            }
            keyPressConsumedByTextInput = false;
        } else if (events[i].type == SDL_KEYUP) {
            onKeyReleased(events[i].key.keysym.sym);
        } else if (events[i].type == SDL_TEXTINPUT) {
            static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::u32string u32text = converter.from_bytes(events[i].text.text);
            bool propagateTextInput = true;
            for (char32_t unicode: u32text) {
                propagateTextInput &= onChar(unicode);
            }
            bool isFollowedByKeyDown = i + 1 < events.size() && events[i + 1].type == SDL_KEYDOWN;
            keyPressConsumedByTextInput = !propagateTextInput && isFollowedByKeyDown;
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

bool Main::onChar(char32_t unicodeCharacter) const {
    //engine
    return context->getScene().onChar(unicodeCharacter);
}

void Main::onKeyPressed(int keyCode, bool isRepeatPress) {
    InputKey key = toInputKey(keyCode);

    //engine
    bool propagateEvent = context->getScene().onKeyPress(key);

    //game
    if (propagateEvent && !isRepeatPress) {
        if (keyCode == SDLK_ESCAPE) {
            context->exit();
        }
        game->onKeyPressed(key);
    }
}

void Main::onKeyReleased(int keyCode) {
    InputKey key = toInputKey(keyCode);

    //engine
    bool propagateEvent = context->getScene().onKeyRelease(key);

    //game
    if (propagateEvent) {
        game->onKeyReleased(key);
    }
}

void Main::onMouseButtonPressed(int button) {
    InputKey key = toInputKey(button);

    //engine
    bool propagateEvent = context->getScene().onKeyPress(key);

    //game
    if (propagateEvent) {
        game->onKeyPressed(key);
    }
}

void Main::onMouseButtonReleased(int button) {
    InputKey key = toInputKey(button);

    //engine
    bool propagateEvent = context->getScene().onKeyRelease(key);

    //game
    if (propagateEvent) {
        game->onKeyReleased(key);
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

InputKey Main::toInputKey(int keyCode) {
    auto itFind = inputKeyMap.find(keyCode);
    if (itFind != inputKeyMap.end()) {
        return itFind->second;
    }

    if (keyCode >= 'a' && keyCode <= 'z') {
        int keyShift = keyCode - 'a';
        return static_cast<InputKey>(static_cast<int>(InputKey::A) + keyShift);
    } else if (keyCode == '=') {
        return InputKey::EQUAL;
    } else if (keyCode == ':') {
        return InputKey::COLON;
    } else if (keyCode == ';') {
        return InputKey::SEMICOLON;
    } else if (keyCode == ',') {
        return InputKey::COMMA;
    } else if (keyCode == '-') {
        return InputKey::MINUS;
    } else if (keyCode == '/') { //for QWERTY keyboards
        return InputKey::SLASH;
    } else if (keyCode == '\\') { //for QWERTY keyboards
        return InputKey::BACKSLASH;
    } else if (keyCode == '\'') { //for QWERTY keyboards
        return InputKey::APOSTROPHE;
    } else if (keyCode == '[') { //for QWERTY keyboards
        return InputKey::LEFT_BRACKET;
    } else if (keyCode == ']') { //for QWERTY keyboards
        return InputKey::RIGHT_BRACKET;
    } else if (keyCode == '.') { //for QWERTY keyboards
        return InputKey::PERIOD;
    } else if (keyCode == '`') { //for QWERTY keyboards
        return InputKey::GRAVE_ACCENT;
    }

    return InputKey::UNKNOWN_KEY;
}

bool Main::argumentsContains(const std::string& argName, std::span<char*> args) const {
    return std::ranges::any_of(args, [&](const char* arg) { return std::string(arg).find(argName) != std::string::npos; });
}

void Main::clearResources(SDL_Window*& window) {
    game.reset(nullptr);
    context.reset(nullptr);

    SDL_Vulkan_UnloadLibrary();
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}
