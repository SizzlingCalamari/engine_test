
#include "Application.h"
#include "SDL.h"
#include <iostream>

namespace ApplicationService
{
    bool Initialize()
    {
        std::cout << "Hello World!" << std::endl;
        SDL_SetMainReady();
        bool ret = (SDL_Init(SDL_INIT_EVERYTHING) == 0);

        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        return ret;
    }

    void Shutdown()
    {
        SDL_Quit();
    }

    Window CreateWindow(const char *title, int x, int y,
                        int w, int h, uint flags)
    {
        auto sdl_win = SDL_CreateWindow(title, x, y, w, h, flags);
        return Window(sdl_win);
    }

    void FreeWindow(Window window)
    {
        SDL_DestroyWindow(window.GetHandle());
    }

    void* CreateGLContext(Window window)
    {
        auto context = SDL_GL_CreateContext(window.GetHandle());
        return context;
    }

    void FreeGLContext(void* GLContext)
    {
        assert(GLContext);
        SDL_GL_DeleteContext(GLContext);
    }

    void FlushAndRefreshEvents()
    {
        SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        SDL_PumpEvents();
    }

    bool QuitRequested()
    {
        return (SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0);
    }

    vector<SDL_Event> GetEventsOfType(SDL_EventType mintype, SDL_EventType maxtype)
    {
        auto num_events = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, mintype, maxtype);
        vector<SDL_Event> events(num_events);

        SDL_PeepEvents(events.data(), num_events, SDL_PEEKEVENT, mintype, maxtype);
        return events;
    }
}
