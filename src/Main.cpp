#include "Main.h"
#include "KeyboardKey.h"
#include "WindowController.h"

int main(int argc, char *argv[])
{
    Main main;
    main.execute(argc, argv);

    return 0;
}

//static
std::list<unsigned int> Main::charPressEvents;
std::list<int> Main::keyPressEvents;

Main::Main() :
        mainDisplayer(nullptr),
        propagatePressKeyEvent(true),
        propagateReleaseKeyEvent(true)
{

}

void Main::execute(int argc, char *argv[])
{
    initializeKeyboardMap();

    urchin::Logger::defineLogger(std::make_unique<urchin::FileLogger>("urchinEngineTest.log"));

    GLFWwindow *window = nullptr;
    WindowController *windowController = nullptr;

    try
    {
        if (!glfwInit())
        {
            //TODO Initialization failed
        }
        glfwSetErrorCallback(glfwErrorCallback);
        
        GLFWmonitor *monitor = argumentsContains("--windowed", argc, argv) ? nullptr : glfwGetPrimaryMonitor();
        window = glfwCreateWindow(1200, 675, "Urchin Engine Test", monitor, nullptr);
        if(!window)
        {
            glfwTerminate();
            //TODO Window or context creation failed
        }
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        windowController = new WindowController(window);
        std::string resourcesDirectory = retrieveResourcesDirectory(argv);
        std::string saveDirectory = retrieveSaveDirectory(argv);

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        mainDisplayer = new MainDisplayer(windowController);
        mainDisplayer->initialize(resourcesDirectory, saveDirectory);
        mainDisplayer->resize(width, height);

        glfwSetWindowUserPointer(window, (void *)this);
        glfwSetCharModsCallback(window, charCallback);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetMouseButtonCallback(window, mouseKeyCallback);
        glfwSetCursorPosCallback(window, cursorPositionCallback);
        glfwSetWindowSizeCallback(window, windowSizeCallback);

        while (!glfwWindowShouldClose(window))
        {
            propagatePressKeyEvent = true;
            propagateReleaseKeyEvent = true;
            glfwPollEvents();
            if(!charPressEvents.empty())
            {
                for(unsigned int charUnicode : charPressEvents)
                {
                    onChar(charUnicode);
                }
                charPressEvents.clear();
            }
            if(!keyPressEvents.empty())
            {
                for(int keyPress : keyPressEvents)
                {
                    onKeyPressed(keyPress);
                }
                keyPressEvents.clear();
            }

            mainDisplayer->paint();

            glfwSwapBuffers(window);
        }

        if(urchin::Logger::logger().hasFailure())
        {
            failureExit(window, windowController);
        }else
        {
            clearResources(window, windowController);
        }
    }catch(std::exception &e)
    {
        urchin::Logger::logger().logError("Error occurred: " + std::string(e.what()));
        failureExit(window, windowController);
    }

    glfwTerminate();
}

void Main::glfwErrorCallback(int error, const char* description)
{ //TODO review
    if(error) {}
    fprintf(stderr, "Error: %s\n", description);
}

void Main::initializeKeyboardMap()
{
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

std::string Main::retrieveResourcesDirectory(char *argv[]) const
{
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "resources/";
}

std::string Main::retrieveSaveDirectory(char *argv[]) const
{
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "save/";
}

void Main::charCallback(GLFWwindow *, unsigned int codepoint, int)
{
    charPressEvents.push_back(codepoint);
}

void Main::keyCallback(GLFWwindow *window, int key, int, int action, int)
{
    if(action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        keyPressEvents.push_back(key);
    }else if(action == GLFW_RELEASE)
    {
        Main *main = (Main*)glfwGetWindowUserPointer(window);
        main->onKeyReleased(key);
    }
}

void Main::mouseKeyCallback(GLFWwindow* window, int button, int action, int)
{
    Main *main = (Main*)glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS)
    {
        main->onMouseButtonPressed(button);
    }else if(action == GLFW_RELEASE)
    {
        main->onMouseButtonReleased(button);
    }
}

void Main::cursorPositionCallback(GLFWwindow* window, double x, double y)
{
    Main *main = (Main*)glfwGetWindowUserPointer(window);
    main->onMouseMove(static_cast<int>(x), static_cast<int>(y));
}

void Main::windowSizeCallback(GLFWwindow *window, int width, int height)
{
    Main *main = (Main*)glfwGetWindowUserPointer(window);
    main->mainDisplayer->resize(width, height);
}

void Main::onChar(unsigned int unicode)
{
    //engine
    if(propagatePressKeyEvent)
    {
        if(unicode < 256)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onChar(unicode);
        }
    }
}

void Main::onKeyPressed(int key)
{
    //engine
    if(propagatePressKeyEvent)
    {
        if(key == GLFW_KEY_LEFT)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::LEFT_ARROW); //TODO rename: keyPressed/Release + accept InputDevice::Key ?
        }else if(key == GLFW_KEY_RIGHT)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::RIGHT_ARROW);
        }
    }

    //game
    if(propagatePressKeyEvent)
    {
        mainDisplayer->onKeyPressed(toKeyboardKey(key));
    }
}

void Main::onKeyReleased(int key)
{
    //engine
    if(propagateReleaseKeyEvent)
    {
        if(key == GLFW_KEY_LEFT)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::LEFT_ARROW);
        }else if(key == GLFW_KEY_RIGHT)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::RIGHT_ARROW);
        }
    }

    //game
    if(propagateReleaseKeyEvent)
    {
        mainDisplayer->onKeyReleased(toKeyboardKey(key));
    }
}

KeyboardKey Main::toKeyboardKey(int key)
{
    if(key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
    {
        int keyShift = glfwGetKeyName(key, 0)[0] - 'a';
        return static_cast<KeyboardKey>(KeyboardKey::A + keyShift);
    }

    auto it = keyboardMap.find(key);
    if(it!=keyboardMap.end())
    {
        return it->second;
    } else
    {
        return KeyboardKey::UNKNOWN_KEY;
    }
}

void Main::onMouseButtonPressed(int button)
{
    //engine
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::MOUSE_LEFT);
    }else if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::MOUSE_RIGHT);
    }
}

void Main::onMouseButtonReleased(int button)
{
    //engine
    if(button == GLFW_MOUSE_BUTTON_LEFT)
    {
        mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::MOUSE_LEFT);
    }else if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::MOUSE_RIGHT);
    }
}

void Main::onMouseMove(int x, int y)
{
    //engine
    if(x!=0 || y!=0)
    {
        mainDisplayer->onMouseMove(x, y);
    }
}

bool Main::argumentsContains(const std::string &argName, int argc, char *argv[]) const
{
    for(int i=1;i<argc;++i)
    {
        if(std::string(argv[i]).find(argName)!=std::string::npos)
        {
            return true;
        }
    }
    return false;
}

void Main::clearResources(GLFWwindow *&window, WindowController *&windowController)
{
    delete mainDisplayer;
    mainDisplayer = nullptr;

    delete windowController;
    windowController = nullptr;

    glfwDestroyWindow(window);
    glfwTerminate();
    window = nullptr;
}

void Main::failureExit(GLFWwindow *&window, WindowController *&windowController)
{
    std::string logFilename = dynamic_cast<urchin::FileLogger&>(urchin::Logger::logger()).getFilename();
    std::cerr<<"Application stopped with issue (log: "<<logFilename<<")"<<std::endl;

    clearResources(window, windowController);

    std::exit(1);
}
