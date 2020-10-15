#include "Main.h"
#include "InputDevice.h"
#include "WindowController.h"

int main(int argc, char *argv[])
{
    Main main;
    main.execute(argc, argv);

    return 0;
}

Main::Main() :
        mainDisplayer(nullptr),
        propagatePressKeyEvent(true),
        propagateReleaseKeyEvent(true)
{

}

void Main::execute(int argc, char *argv[])
{
    initializeKeyboardMap();

    sf::ContextSettings settings;
    settings.depthBits = 0;
    settings.stencilBits = 0;
    settings.antialiasingLevel = 0;
    settings.majorVersion = 4;
    settings.minorVersion = 5;

    urchin::Logger::defineLogger(std::make_unique<urchin::FileLogger>("urchinEngineTest.log"));

    sf::Window *window = nullptr;
    WindowController *windowController = nullptr;

    try
    {
        if (argumentsContains("--windowed", argc, argv))
        {
            sf::VideoMode defaultVideoModelSize(1200, 675);
            window = new sf::Window(defaultVideoModelSize, "Urchin Engine Demo", sf::Style::Default, settings);
        } else
        {
            sf::VideoMode desktopModelSize = sf::VideoMode::getDesktopMode();
            window = new sf::Window(desktopModelSize, "Urchin Engine Demo", sf::Style::Fullscreen, settings);
        }

        windowController = new WindowController(window);
        std::string resourcesDirectory = retrieveResourcesDirectory(argv);
        std::string saveDirectory = retrieveSaveDirectory(argv);

        mainDisplayer = new MainDisplayer(windowController);
        mainDisplayer->initialize(resourcesDirectory, saveDirectory);
        mainDisplayer->resize(window->getSize().x, window->getSize().y);

        windowController->cleanEvents(); //ignore events occurred during initialize phase

        bool running = true;
        while (running)
        {
            propagatePressKeyEvent = true;
            propagateReleaseKeyEvent = true;
            std::list<sf::Event> events = retrieveOrderedEvents(window);
            while (!events.empty())
            {
                sf::Event event = events.back();
                events.pop_back();

                if (event.type == sf::Event::MouseMoved)
                {
                    onMouseMove(event);
                } else if (event.type == sf::Event::TextEntered)
                {
                    onChar(event);
                } else if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Escape)
                    {
                        running = false;
                    }

                    onKeyPressed(event);
                } else if (event.type == sf::Event::KeyReleased)
                {
                    onKeyReleased(event);
                } else if (event.type == sf::Event::MouseButtonPressed)
                {
                    onMouseButtonPressed(event);
                } else if (event.type == sf::Event::MouseButtonReleased)
                {
                    onMouseButtonReleased(event);
                }
                if (event.type == sf::Event::Closed)
                {
                    running = false;
                } else if (event.type == sf::Event::Resized)
                {
                    mainDisplayer->resize(event.size.width, event.size.height);
                }
            }

            mainDisplayer->paint();

            window->display();
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
}

void Main::initializeKeyboardMap()
{
    keyboardMap[sf::Keyboard::Escape] = InputDevice::Key::ESCAPE;
    keyboardMap[sf::Keyboard::Space] = InputDevice::Key::SPACE;
    keyboardMap[sf::Keyboard::LControl] = InputDevice::Key::CTRL_LEFT;
    keyboardMap[sf::Keyboard::RControl] = InputDevice::Key::CTRL_RIGHT;
    keyboardMap[sf::Keyboard::LAlt] = InputDevice::Key::ALT_LEFT;
    keyboardMap[sf::Keyboard::RAlt] = InputDevice::Key::ALT_RIGHT;
    keyboardMap[sf::Keyboard::LShift] = InputDevice::Key::SHIFT_LEFT;
    keyboardMap[sf::Keyboard::RShift] = InputDevice::Key::SHIFT_RIGHT;
    keyboardMap[sf::Keyboard::Left] = InputDevice::Key::ARROW_LEFT;
    keyboardMap[sf::Keyboard::Right] = InputDevice::Key::ARROW_RIGHT;
    keyboardMap[sf::Keyboard::Up] = InputDevice::Key::ARROW_UP;
    keyboardMap[sf::Keyboard::Down] = InputDevice::Key::ARROW_DOWN;
    keyboardMap[sf::Keyboard::PageUp] = InputDevice::Key::PAGE_UP;
    keyboardMap[sf::Keyboard::PageDown] = InputDevice::Key::PAGE_DOWN;
}

std::string Main::retrieveResourcesDirectory(char *argv[]) const
{
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "resources/";
}

std::string Main::retrieveSaveDirectory(char *argv[]) const
{
    return urchin::FileHandler::getDirectoryFrom(std::string(argv[0])) + "save/";
}

std::list<sf::Event> Main::retrieveOrderedEvents(sf::Window *window) const
{
    std::list<sf::Event> eventsOrderedList;

    sf::Event event{};
    while (window->pollEvent(event))
    {
        if(event.type != sf::Event::TextEntered)
        { //onChar event must be treated before onKeyPressed event
            eventsOrderedList.push_front(event);
        }else
        {
            eventsOrderedList.push_back(event);
        }
    }

    return eventsOrderedList;
}

void Main::onChar(const sf::Event &event)
{
    //engine
    if(propagatePressKeyEvent)
    {
        if(event.text.unicode < 256)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onChar(event.text.unicode);
        }
    }
}

void Main::onKeyPressed(const sf::Event &event)
{
    //engine
    if(propagatePressKeyEvent)
    {
        if(event.key.code == sf::Keyboard::Left)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::LEFT_ARROW);
        }else if(event.key.code == sf::Keyboard::Right)
        {
            propagatePressKeyEvent = mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::RIGHT_ARROW);
        }
    }

    //game
    if(propagatePressKeyEvent)
    {
        if(event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
        {
            int gameKey = (event.key.code - sf::Keyboard::A) + InputDevice::Key::A;
            mainDisplayer->onKeyPressed(static_cast<InputDevice::Key>(gameKey));
        }else
        {
            auto it = keyboardMap.find(event.key.code);
            if(it!=keyboardMap.end())
            {
                mainDisplayer->onKeyPressed(it->second);
            }
        }
    }
}

void Main::onKeyReleased(const sf::Event &event)
{
    //engine
    if(propagateReleaseKeyEvent)
    {
        if(event.key.code == sf::Keyboard::Left)
        {
            propagateReleaseKeyEvent = mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::LEFT_ARROW);
        }else if(event.key.code == sf::Keyboard::Right)
        {
            propagateReleaseKeyEvent = mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::RIGHT_ARROW);
        }
    }

    //game
    if(propagateReleaseKeyEvent)
    {
        if(event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
        {
            int gameKey = (event.key.code - sf::Keyboard::A) + InputDevice::Key::A;
            mainDisplayer->onKeyReleased(static_cast<InputDevice::Key>(gameKey));
        }else
        {
            auto it = keyboardMap.find(event.key.code);
            if(it!=keyboardMap.end())
            {
                mainDisplayer->onKeyReleased(it->second);
            }
        }
    }
}

void Main::onMouseButtonPressed(const sf::Event &event)
{
    //engine
    if(event.mouseButton.button == sf::Mouse::Left)
    {
        mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::MOUSE_LEFT);
    }else if(event.mouseButton.button == sf::Mouse::Right)
    {
        mainDisplayer->getSceneManager()->onKeyDown(urchin::InputDevice::Key::MOUSE_RIGHT);
    }
}

void Main::onMouseButtonReleased(const sf::Event &event)
{
    //engine
    if(event.mouseButton.button == sf::Mouse::Left)
    {
        mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::MOUSE_LEFT);
    }else if(event.mouseButton.button == sf::Mouse::Right)
    {
        mainDisplayer->getSceneManager()->onKeyUp(urchin::InputDevice::Key::MOUSE_RIGHT);
    }
}

void Main::onMouseMove(const sf::Event &event)
{
    //engine
    if(event.mouseMove.x!=0 || event.mouseMove.y!=0)
    {
        mainDisplayer->onMouseMove(event.mouseMove.x, event.mouseMove.y);
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

void Main::clearResources(sf::Window *&window, WindowController *&windowController)
{
    if(window->isOpen())
    {
        window->close();
    }

    delete mainDisplayer;
    mainDisplayer = nullptr;

    delete windowController;
    windowController = nullptr;

    delete window;
    window = nullptr;
}

void Main::failureExit(sf::Window *&window, WindowController *&windowController)
{
    std::string logFilename = dynamic_cast<urchin::FileLogger&>(urchin::Logger::logger()).getFilename();
    std::cerr<<"Application stopped with issue (log: "<<logFilename<<")"<<std::endl;

    window->close();
    clearResources(window, windowController);

    std::exit(1);
}
