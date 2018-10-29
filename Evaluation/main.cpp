#include <iostream>
#include "main/Game.h"

int main(int argc, char** argv) {	
	auto frequency = SDL_GetPerformanceFrequency();
	GameTime preferredFrameDuration = frequency / Config.FPS;
	GameTime initialFrame;
	GameTime frameDuration;

	String settings = argc > 1 ? String(argv[1]) : Config.DefaultSettings;
	
	Game* game = new Game();
	if (game->initialize(settings)) {
		game->run(SDL_GetPerformanceCounter());
		while (game->isRunning()) {

			initialFrame = SDL_GetPerformanceCounter();

			game->handleEvents();
			game->update(initialFrame);
			game->render();

			frameDuration = SDL_GetPerformanceCounter() - initialFrame;

			if (frameDuration < preferredFrameDuration) {
				SDL_Delay((preferredFrameDuration - frameDuration) * 1000 / frequency);
			}
		}
	}
	else {
		return 1;
	}
	game->dispose();
	delete game;
	return 0;
}