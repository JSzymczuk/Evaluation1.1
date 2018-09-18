#include <iostream>
#include "main/Game.h"

int main(int argc, char** argv) {
	
	int preferredFrameDuration = 1000 / Config.FPS;
	int initialFrame;
	int frameDuration;

	String settings = argc > 1 ? String(argv[1]) : Config.DefaultSettings;

	Game* game = new Game();
	if (game->initialize(settings)) {
		while (game->isRunning()) {

			initialFrame = SDL_GetTicks();

			game->handleEvents();
			game->update();
			game->render();

			frameDuration = SDL_GetTicks() - initialFrame;

			if (frameDuration < preferredFrameDuration) {
				SDL_Delay(preferredFrameDuration - frameDuration);
			}
		}
		game->dispose();
		return 0;
	}
	else {
		return 1;
	}
}