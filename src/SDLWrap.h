
#pragma once

#include <list>
#include <array>
#include <functional>
#include <vector>

#include "SDLWindow.h"
#include "SDL_events.h"

// SDL.h
enum InitFlags
{
    INIT_TIMER =         0x00000001,
    INIT_AUDIO =         0x00000010,
    // video or joystick implies events
    INIT_VIDEO =         0x00000020,
    INIT_JOYSTICK =      0x00000200,
    INIT_HAPTIC =        0x00001000,
    // gamecontroller implies joystick
    INIT_GAMECONTROLER = 0x00002000,
    INIT_EVENTS =        0x00004000,
    INIT_EVERYTHING = 
        INIT_TIMER | INIT_AUDIO | INIT_VIDEO | INIT_JOYSTICK |
        INIT_HAPTIC | INIT_GAMECONTROLER | INIT_EVENTS
};

typedef std::function<void(const SDL_KeyboardEvent&)> KeyboardCallback;

class SDLWrap
{
public:
    SDLWrap() = default;
    ~SDLWrap() = default;

    bool Init(uint flags);
    void Shutdown();

    auto CreateWindow(const char *title, int x, int y,
                      int w, int h, uint flags) -> SDLWindow;

    auto CreateGLContext(SDLWindow *window) -> void*;

    bool ProcessEvents();

    void RegisterKeyboardEventListener(KeyboardCallback fn)
    {
        m_keyboard_listeners.emplace_back(std::move(fn));
    }

private:
    auto GrabEvents() -> int;

private:
    static const uint MAX_EVENTS_PER_ITERATION = 20;

private:
    std::list<SDL_GLContext> m_glcontexts;
    std::list<SDL_Window*> m_windows;
    std::array<SDL_Event, MAX_EVENTS_PER_ITERATION> m_events;

    std::vector<KeyboardCallback> m_keyboard_listeners;
};
