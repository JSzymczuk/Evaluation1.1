#pragma once

#include <string>
#include <condition_variable>
#include <thread>
#include "Configuration.h"
#include "math/Math.h"
#include "engine/SegmentTree.h"
#include "engine/Navigation.h"
#include "engine/MissileManager.h"
#include "entities/Team.h"
#include "entities/Trigger.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "agents/Agent.h"
#include "agents/LuaEnvironment.h"
#include "engine/Camera.h"

enum GameState {
	IN_PROGRESS,
	ENDED_WIN,
	ENDED_DRAW,
	TIME_OUT
};

struct ActorLoadedData {
	String name;
	String script;
	size_t team;
	Vector2 position;
};

class Game {
public:
	Game();
	~Game();
	bool initialize(const String& settings);
	bool isRunning();
	void handleEvents();
	void run(GameTime time);
	void update(GameTime time);
	void render() const;
	void dispose();

	static Game* getInstance();
	static GameTime getCurrentTime();
	
	GameTime getTime() const;
	GameMap* getMap() const;
	std::vector<Team*> getTeams() const;
	std::vector<Actor*> getActors() const;
	std::vector<Trigger*> getTriggers() const;
	MissileManager* getMissileManager() const;

	void registerAgentToDispose(Agent* agent);
	GameState checkWinLoseConditions(std::vector<Team*>& winners) const;
	GameTime getRemainingTime() const;

private:
	static Game* _instance;

	GameTime _gameTime;
	GameTime _timeStarted;
	GameTime _timeEnd;
	std::mutex _updateMutex;
	std::condition_variable _updateHolder;

	bool _isRunning;
	SDL_Window* _window;
	SDL_Renderer* _renderer;
	Camera* _camera;
	
	LuaEnv* _luaEnv;
	GameMap* _gameMap;
	MissileManager* _missileManager;
	std::vector<Team*> _teams;

	PlayerAgent* _playerAgent;
	std::vector<Agent*> _agents;
	std::queue<Agent*> _threadsToDispose;

	void initializeTeams(const std::vector<ActorLoadedData>& actorsData);

	bool _areHealthBarsVisible = true;
	bool _isNavigationMeshVisible = false;
	bool _areAabbsVisible = false;
	bool _iscurrentPathVisible = false;
	bool _isUpdateEnabled = true;
	std::queue<Vector2> _path;
	size_t _lastTimeVisible;
	size_t _duration;
	size_t _fps;
	size_t _lastFps;

	int mousePosX;
	int mousePosY;

	friend class Agent;

};
