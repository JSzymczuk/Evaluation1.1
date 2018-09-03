#pragma once

#include <string>
#include <unordered_map>
#include <SDL.h>
#include <SDL_ttf.h>

class ResourceManager {
public:
	void loadImage(const std::string& key, const std::string& filename);
	bool hasImage(const std::string& key) const;
	SDL_Surface* getImage(const std::string& key) const;
	
	void loadFont(const std::string& key, const std::string& filename, int size);
	bool hasFont(const std::string& key) const;
	TTF_Font* getFont(const std::string& key) const;

	static bool initialize();
	static void dispose();
	static ResourceManager* get();

private:
	std::unordered_map<std::string, SDL_Surface*> _textures;
	std::unordered_map<std::string, TTF_Font*> _fonts;

	ResourceManager();
	~ResourceManager();

	static ResourceManager* _instance;
};
