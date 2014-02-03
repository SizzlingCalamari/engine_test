
#pragma once

#include <cstdint>
#include <list>

#include "GLContext.h"
#include "SDL.h"

class SDLWrap
{
public:
    SDLWrap() = default;
    ~SDLWrap() = default;

    bool Init(uint32_t flags);
    void Shutdown();

    SDL_Window *CreateWindow(
        const char *title,
        int x, int y, int w,
        int h, Uint32 flags);

    GLContext CreateGLContext(SDL_Window *window);

private:
    std::list<SDL_GLContext> m_glcontexts;
    std::list<SDL_Window*> m_windows;
};
