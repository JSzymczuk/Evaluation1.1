#include <iostream>
#include "main/Game.h"

int main(int argc, char** argv) {

	//GameMap::Loader().generateConnections("map1.map", MapsDirectory + "/map2.map");

	int preferredFrameDuration = 1000000 / FPS;
	int initialFrame;
	int frameDuration;

	Game* game = new Game();
	if (game->initialize("", DisplayWidth, DisplayHeight)) {

		while (game->isRunning()) {

			initialFrame = SDL_GetPerformanceCounter();

			game->handleEvents();
			game->update(initialFrame);
			game->render();

			frameDuration = SDL_GetPerformanceCounter() - initialFrame;

			if (frameDuration < preferredFrameDuration) {
				SDL_Delay((preferredFrameDuration - frameDuration) / 1000);
			}
		}
		game->dispose();
		return 0;
	}
	else {
		return 1;
	}
	/**/

}