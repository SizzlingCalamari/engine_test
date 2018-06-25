
#pragma once

typedef struct SDL_Window SDL_Window;

class Window
{
public:
    explicit Window(SDL_Window *window = nullptr):
        m_window(window)
    {
    }

    void SwapBuffers();

    SDL_Window *GetHandle()
    {
        return m_window;
    }

private:
    SDL_Window *m_window;
};
