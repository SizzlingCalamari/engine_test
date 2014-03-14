
#include "SDLWrap.h"
#include "SDL.h"

static_assert(INIT_TIMER == SDL_INIT_TIMER, "INIT_TIMER incorrect");
static_assert(INIT_AUDIO == SDL_INIT_AUDIO, "INIT_AUDIO incorrect");
static_assert(INIT_VIDEO == SDL_INIT_VIDEO, "INIT_VIDEO incorrect");
static_assert(INIT_JOYSTICK == SDL_INIT_JOYSTICK, "INIT_JOYSTICK incorrect");
static_assert(INIT_HAPTIC == SDL_INIT_HAPTIC, "INIT_HAPTIC incorrect");
static_assert(INIT_GAMECONTROLER == SDL_INIT_GAMECONTROLLER, "INIT_GAMECONTROLER incorrect");
static_assert(INIT_EVENTS == SDL_INIT_EVENTS, "INIT_EVENTS incorrect");
static_assert(INIT_EVERYTHING == SDL_INIT_EVERYTHING, "INIT_EVERYTHING incorrect");

bool SDLWrap::Init(uint flags)
{
    SDL_SetMainReady();
    return (SDL_Init(flags) == 0);
}

void SDLWrap::Shutdown()
{
    for (auto context : m_glcontexts)
    {
        SDL_GL_DeleteContext(context);
    }
    for (auto window : m_windows)
    {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

SDLWindow SDLWrap::CreateWindow(
    const char *title,
    int x, int y, int w,
    int h, uint flags)
{
    SDL_Window *win = SDL_CreateWindow(title, x, y, w, h, flags);
    m_windows.emplace_back(win);
    return SDLWindow(win);
}

void *SDLWrap::CreateGLContext(SDLWindow *window)
{
    SDL_Window *w = window->GetHandle();
    SDL_GLContext c = SDL_GL_CreateContext(w);

    m_glcontexts.emplace_back(c);
    return c;
}

bool SDLWrap::ProcessEvents()
{
    int num_events = 0;
    bool quit = false;

    SDL_PumpEvents();
    while ((num_events = GrabEvents()) > 0)
    {
        for (int i = 0; i < num_events; ++i)
        {
            uint32 type = m_events[i].type;
            quit = quit || (type == SDL_QUIT);
        }
    }
    return !quit;
}

int SDLWrap::GrabEvents()
{
    return SDL_PeepEvents(m_events.data(), MAX_EVENTS_PER_ITERATION,
        SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT);
}
