
#include <cstdint>
#include <iostream>

#include "World.h"

int main(int argc, char *argv[])
{
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
}
