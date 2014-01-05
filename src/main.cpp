
#include <cstdint>
#include <iostream>

#include "World.h"
#include "SDL.h"

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    std::cout << "Hello World!" << std::endl;

    World w;
    w.Initialize();

	const uint32_t SERVER_FRAME_DT = 10000;

	bool quit = false;
	while (!quit)
	{
		w.Update(SERVER_FRAME_DT);
	}
    w.Shutdown();
	SDL_Quit();
}
