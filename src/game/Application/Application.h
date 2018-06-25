
#pragma once

#include "Window.h"
#include "SDL_events.h"
#include <vector>

namespace ApplicationService
{
    bool Initialize();
    void Shutdown();

    Window CreateWindow(const char *title, int x, int y,
                        int w, int h, uint flags);
    void FreeWindow(Window window);

    void* CreateGLContext(Window window);
    void FreeGLContext(void* GLContext);

    void FlushAndRefreshEvents();
    bool QuitRequested();
    
    std::vector<SDL_Event> GetEventsOfType(SDL_EventType mintype, SDL_EventType maxtype);
}
