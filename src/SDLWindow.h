
#pragma once

//#include "SDL_video.h"
typedef struct SDL_Window SDL_Window;

class SDLWindow
{
    friend class SDLWrap;

public:
    SDLWindow(SDL_Window *window);
    ~SDLWindow() = default;

    void SwapBuffers();

protected:
    SDL_Window *GetHandle();

private:
    SDL_Window *m_window;
};

inline SDL_Window *SDLWindow::GetHandle()
{
    return m_window;
}
