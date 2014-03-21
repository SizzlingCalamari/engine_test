
#include "Window.h"
#include "SDL_video.h"

void Window::SwapBuffers()
{
    SDL_GL_SwapWindow(m_window);
}
