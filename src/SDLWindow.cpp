
#include "SDLWindow.h"
#include "SDL_video.h"

SDLWindow::SDLWindow(SDL_Window *window):
    m_window(window)
{
}

void SDLWindow::SwapBuffers()
{
    SDL_GL_SwapWindow(m_window);
}
