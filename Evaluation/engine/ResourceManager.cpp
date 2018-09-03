#include <SDL_image.h>
#include "ResourceManager.h"
#include "Logger.h"
#include <iostream>

ResourceManager* ResourceManager::_instance = nullptr;

ResourceManager::ResourceManager() {}

ResourceManager* ResourceManager::get() { return _instance; }

ResourceManager::~ResourceManager() {
	for (auto it = _textures.begin(); it != _textures.end(); ++it) {
		SDL_FreeSurface(it->second);
	}
	for (auto it = _fonts.begin(); it != _fonts.end(); ++it) {
		TTF_CloseFont(it->second);
	}
}

void ResourceManager::loadImage(const std::string& key, const std::string& filename) {
	try {
		if (hasImage(key)) { throw "Klucz " + key + " juz zostal przypisany.";  }
		SDL_Surface* surface = IMG_Load(filename.c_str());
		if (surface != nullptr) {
			_textures.insert({ key, surface });
		}
		else { throw SDL_GetError(); }
	}
	catch (const char* s) {
		Logger::log("Blad podczas wczytywania pliku '" + filename + "': " + s + ".");
	}
}

void ResourceManager::loadFont(const std::string& key, const std::string& filename, int size) {
	try {
		if (hasFont(key)) { throw; }
		TTF_Font* font = TTF_OpenFont(filename.c_str(), size);
		if (font != nullptr) {
			_fonts.insert({ key, font });
		}
		else { throw; }
	}
	catch (...) {
		throw "Blad podczas wczytywania pliku '" + filename + "'.";
	}
}

bool ResourceManager::hasImage(const std::string& key) const { return _textures.find(key) != _textures.end(); }

bool ResourceManager::hasFont(const std::string& key) const { return _fonts.find(key) != _fonts.end(); }

SDL_Surface* ResourceManager::getImage(const std::string& key) const {
	auto result = _textures.find(key);
	return result != _textures.end() ? result->second : nullptr;
}

TTF_Font* ResourceManager::getFont(const std::string& key) const {
	auto result = _fonts.find(key);
	return result != _fonts.end() ? result->second : nullptr;
}

bool ResourceManager::initialize() {
	bool success = true;
	if (_instance == nullptr) {
		_instance = new ResourceManager();
		if (TTF_Init() != 0) {
			std::cout << "Blad inicjalizacji czcionek: " + String(SDL_GetError()) + "." << std::endl;
			success = false;
		}
		if (!IMG_Init(IMG_INIT_PNG)) {
			std::cout << "Blad inicjalizacji bibliteki png: " + String(SDL_GetError()) + "." << std::endl;
			success = false;
		}
	}
	else { std::cout << "Manager zasobow juz zostal zainicjowany."; }
	return success;
}

void ResourceManager::dispose() {
	if (_instance != nullptr) {
		delete _instance;
		_instance = nullptr;
		TTF_Quit();
		IMG_Quit();
	}
}
