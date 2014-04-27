
#include <iostream>
#include "Application/Application.h"

#include "Engine.h"

/*void callback(const KeyboardEventInfo& info)
{
    ButtonCombination b = info.buttons;
    if (b.m_buttons[0])
    {
        std::cout << b.m_buttons[0] << " ";
    }
    if (info.buttons.m_buttons[1])
    {
        std::cout << b.m_buttons[1] << " ";
    }
    if (info.buttons.m_buttons[2])
    {
        std::cout << b.m_buttons[2] << " ";
    }
    if (info.buttons.m_buttons[3])
    {
        std::cout << b.m_buttons[3] << " ";
    }

    if (info.pressed)
    {
        std::cout << "down";
    }
    else
    {
        std::cout << "up";
    }
    std::cout << std::endl;
}*/

#include <type_traits>

using EngineStorage = std::aligned_storage<sizeof(Engine), 16U>::type;

int main(int argc, const char *argv[])
{
    ApplicationService::Initialize();

    /*KeyboardContext k;
    k.AddMapping({ SDL_SCANCODE_A }, &::callback);
    k.AddMapping({ SDL_SCANCODE_S }, &::callback);
    k.AddMapping({ SDL_SCANCODE_D }, &::callback);
    k.AddMapping({ SDL_SCANCODE_W }, &::callback);
    k.AddMapping({ SDL_SCANCODE_LCTRL, SDL_SCANCODE_X }, &::callback);

    InputMapper input;
    input.LoadContext(std::move(k));*/

    auto *engine_storage = new EngineStorage;

    Engine *engine = new(engine_storage) Engine;
    engine->Initialize();
    engine->Run();
    engine->Shutdown();
    engine->~Engine();

    delete engine_storage;

    ApplicationService::Shutdown();
    return 0;
}
