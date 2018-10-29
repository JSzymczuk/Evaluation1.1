#include "Game.h"
#include "engine/ResourceManager.h"
#include "engine/Rng.h"
#include <iostream>
#include <SDL_image.h>
#include "actions/Action.h"
#include "engine/TriggerFactory.h"
#include "entities/Actor.h"
#include "entities/Wall.h"
#include "actions/ChangeWeapon.h"
#include "actions/Dead.h"
#include "actions/Die.h"
#include "actions/Face.h"
#include "actions/Move.h"
#include "actions/Shoot.h"
#include "agents/LuaEnvironment.h"
#include "engine/CommonFunctions.h"
#include <fstream>
#include "engine/TreeCollisionResolver.h"


std::map<Agent*, size_t> agentsFps;
std::map<Agent*, size_t> agentsTotalFrames;

Game::Game() {
	if (_instance != nullptr) {
		delete _instance;
	}
	_instance = this;
	_camera = nullptr;
}

Game::~Game() {
	for (Agent* a : _agents) {
		delete a;
	}

	if (_camera != nullptr) { delete _camera; }

	GameMap::destroy(_gameMap);
	destroyLuaEnv(_luaEnv);

	if (_instance == this) { _instance = nullptr; }
}

Game* Game::_instance = nullptr;

Game* Game::getInstance() { return _instance; }

GameTime Game::getCurrentTime() { return _instance->_gameTime; }

MissileManager* Game::getMissileManager() const { return _missileManager; }

GameTime Game::getTime() const { return _gameTime; }

GameMap* Game::getMap() const { return _gameMap; }

std::vector<Team*> Game::getTeams() const { return _instance->_teams; }

std::vector<Actor*> Game::getActors() const { return _instance->_gameMap->getActors(); }

std::vector<Trigger*> Game::getTriggers() const { return _instance->_gameMap->getTriggers(); }

struct GameSettings {
	size_t duration;
	String map;
	std::vector<ActorLoadedData> actors;
};

GameSettings loadActorsData(String actorsFilename) {
	std::ifstream reader;
	GameSettings settings;
	reader.open(actorsFilename);

	if (reader.fail()) {
		Logger::log("Plik '" + actorsFilename + "' nie istnieje, jest niedostêpny lub uszkodzony.");
	}
	else {
		String name, script;
		size_t team;
		float x, y;

		if (reader >> name >> team) {
			settings.map = name;
			settings.duration = team;
		}

		while (reader >> name >> script >> team >> x >> y) {
			settings.actors.push_back(ActorLoadedData{ name, script, team, Vector2(x, y) });
		}
		reader.close();
	}

	return settings;
}

bool Game::initialize(const String& settingsFilename) {
	if (!SDL_Init(SDL_INIT_EVERYTHING)) {
		_window = SDL_CreateWindow(Config.WindowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			Config.DisplayWidth, Config.DisplayHeight, SDL_WINDOW_SHOWN);
		if (_window) {
			_renderer = SDL_CreateRenderer(_window, -1, 0);
			if (_renderer) {
				_isRunning = true;
			}
		}
	}
	else {
		_isRunning = false;
	}
	
	auto settings = loadActorsData(settingsFilename);
	
	if (!ResourceManager::initialize()) { return false; }
	TriggerFactory::initialize();
	_luaEnv = createLuaEnv();
	_playerAgent = nullptr;

	if (Config.AreLogsVisible) {
		Logger::startLogging();
	}
	else {
		Logger::stopLogging();
	}

	auto rm = ResourceManager::get();
	rm->loadFont("mainfont", "content/CourierNew.ttf", 14);
	rm->loadImage(Config.ActorRingTextureKey, Config.ActorRingTexturePath);
	rm->loadImage(Config.TriggerRingTextureKey, Config.TriggerRingTexturePath);

	_gameMap = GameMap::create(settings.map.c_str());
	
	_missileManager = new MissileManager();
	_missileManager->initialize(_gameMap);
	
	int mapHeight = _gameMap->getHeight();
	int mapWidth = _gameMap->getWidth();
	int displayWidth = Config.DisplayWidth;
	int displayHeight = Config.DisplayHeight;
	int dw = -displayWidth / 2;
	int dh = -displayHeight / 2;

	_camera = new Camera(-(displayWidth - mapWidth) / 2, -(displayHeight - mapHeight) / 2,
		dw, mapWidth + dw, dh, mapHeight + dh);

	initializeTeams(settings.actors);

	_duration = settings.duration;
	_fps = 0;
	_lastFps = 0;

	return true;
}

void Game::run(GameTime time) {
	if (Config.MultithreadingEnabled) {
		for (Agent* agent : _agents) {
			agent->run(time);
		}
	}
	else {
		for (Agent* agent : _agents) {
			agent->initialize(time);
		}
	}
	_timeStarted = time;
	_timeEnd = _timeStarted + SDL_GetPerformanceFrequency() * _duration;
	_gameTime = _timeStarted;
	_lastTimeVisible = _duration;
}

void Game::initializeTeams(const std::vector<ActorLoadedData>& actorsData) {
	// Actor* actor = new Actor("test" + std::to_string(i), 0, center + Vector2(cosf(i * angle), sinf(i * angle)) * r);
	std::map<size_t, Team*> teams;
	std::map<Team*, std::vector<Agent*>> teamAgents;
	std::vector<DynamicEntity*> actors;

	for (auto actorData : actorsData) {
		Team* team = nullptr;
		auto teamIter = teams.find(actorData.team);
		if (teamIter != teams.end()) {
			team = teamIter->second;
		}
		else {
			team = new Team(actorData.team);
			teams[actorData.team] = team;
			teamAgents[team] = std::vector<Agent*>();
		}

		Actor* actor = new Actor(actorData.name, actorData.position);
		size_t agentScriptPrefixLength = Config.AgentScriptPrefix.length();
		bool isValid = false;
		Agent* agent;

		if (_gameMap->canPlace(actor)) {
			if (_playerAgent == nullptr && actorData.script == Config.AgentControlled) {
				_playerAgent = new PlayerAgent(actor);
				agent = _playerAgent;
				isValid = true;
			}
			else if (actorData.script.substr(0, agentScriptPrefixLength) == Config.AgentScriptPrefix) {
				agent = new LuaAgent(actor, actorData.script.substr(agentScriptPrefixLength), _luaEnv);
				isValid = true;
			}
		}

		if (isValid) {
			_agents.push_back(agent);
			team->addMember(actorData.name, actor);
			teamAgents[team].push_back(agent);
			actors.push_back(actor);

			agentsFps[agent] = 0;
			agentsTotalFrames[agent] = 0;
		}
		else {
			delete actor;
		}
	}

	_gameMap->initializeDynamic(actors);

	std::vector<SDL_Color> teamColors = { 
		{ 47, 63, 191 }, // niebieski
		{ 191, 31, 31 }, // czerwony
		{ 47, 143, 47 }, // zielony
		{ 191, 191, 47 }, // ¿ó³ty
		{ 159, 32, 207 }, // fioletowy
		{ 47, 143, 143 }, // turkusowy
		{ 191, 63, 191 }, // ró¿owy
		{ 159, 95,  79 }, // br¹zowy
		{ 143, 179, 191 }, // srebrny
		{ 207, 143,  63 }, // z³oty
	};

	int i = 0;
	Uint8 r, g, b;

	for (auto entry : teams) {
		if (entry.second->getSize() > 0) {
			_teams.push_back(entry.second);
			if (i < teamColors.size()) {
				entry.second->setColor(teamColors[i]);
				++i;
			} 
			else {
				r = Rng::getInteger(0, 255);
				g = Rng::getInteger(0, 255);
				b = Rng::getInteger(0, 255);
				entry.second->setColor({ r, g, b });
			};
		}
		else {
			delete entry.second;
		}
	}

	for (Team* team : _teams) {
		std::vector<Agent*> agents = teamAgents.at(team);
		for (Agent* a1 : agents) {
			for (Agent* a2 : agents) {
				if (a1 != a2) {
					a1->addNotificationListener(a2);
					a2->addNotificationSender(a1);
				}
			}
		}
	}	
}

bool Game::isRunning() { return _isRunning; }

void Game::handleEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

		switch (event.type) {
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&mousePosX, &mousePosY);
			break;

		case SDL_MOUSEBUTTONUP:
			SDL_GetMouseState(&mousePosX, &mousePosY);

			if (event.button.button == SDL_BUTTON_LEFT) {
				if (_playerAgent != nullptr) {
					_playerAgent->shoot(Vector2(mousePosX, mousePosY));
				}
			}
			else if (event.button.button == SDL_BUTTON_RIGHT) {
				if (_playerAgent != nullptr) {
					if (keyboardState[SDL_SCANCODE_LSHIFT]) {
						_playerAgent->moveDirection(Vector2(mousePosX, mousePosY) - _playerAgent->getActor()->getPosition());
					}
					else if (keyboardState[SDL_SCANCODE_LALT]) {
						_playerAgent->face(Vector2(mousePosX, mousePosY));
					}
					else if (keyboardState[SDL_SCANCODE_LCTRL]) {
						_playerAgent->wait();
					}
					else if (keyboardState[SDL_SCANCODE_W]) {
						_playerAgent->wander();
					}
					else {
						_playerAgent->move(Vector2(mousePosX, mousePosY));
					}
				}
			}
			break;

		case SDL_KEYDOWN: {
			const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);

			if (_playerAgent != nullptr) {
				if (keyboardState[SDL_SCANCODE_1]) {
					_playerAgent->selectWeapon("Shotgun");
				}
				else if (keyboardState[SDL_SCANCODE_2]) {
					_playerAgent->selectWeapon("Railgun");
				}
				else if (keyboardState[SDL_SCANCODE_3]) {
					_playerAgent->selectWeapon("Chaingun");
				}
				else if (keyboardState[SDL_SCANCODE_4]) {
					_playerAgent->selectWeapon("RocketLauncher");
				}
			}

			if (keyboardState[SDL_SCANCODE_G]) {
				_isNavigationMeshVisible = !_isNavigationMeshVisible;
			}

			if (keyboardState[SDL_SCANCODE_SPACE]) {
				_areAabbsVisible = !_areAabbsVisible;
			}

			if (keyboardState[SDL_SCANCODE_P]) {
				_isUpdateEnabled = !_isUpdateEnabled;
			}

			if (keyboardState[SDL_SCANCODE_V]) {
				_areHealthBarsVisible = !_areHealthBarsVisible;
			}

			if (keyboardState[SDL_SCANCODE_LCTRL] && keyboardState[SDL_SCANCODE_L]) {
				if (Logger::isLogging()) { Logger::stopLogging(); }
				else { Logger::startLogging(); }
			}

			_camera->update(keyboardState[SDL_SCANCODE_UP], keyboardState[SDL_SCANCODE_RIGHT],
				keyboardState[SDL_SCANCODE_DOWN], keyboardState[SDL_SCANCODE_LEFT]);

			break;
		}
		case SDL_QUIT:
			_isRunning = false;
			break;
		default: break;
		}
	}
}


void Game::dispose() {
	delete _missileManager;
	GameMap::destroy(_gameMap);
	ResourceManager::dispose();
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Game::update(GameTime time) {
	if (_isUpdateEnabled) {
		_gameTime = time;
		if (_gameTime > _timeEnd) {
			_gameTime = _timeEnd;
		}

		size_t remainingTimeInSeconds = getRemainingTime();

		if (remainingTimeInSeconds < _lastTimeVisible) {
			_lastTimeVisible = remainingTimeInSeconds;
			_lastFps = _fps;
			_fps = 1;
			
			for (Agent* agent : _agents) {
				size_t newTotalFrames = agent->getTotalFrames();
				agentsFps[agent] = newTotalFrames - agentsTotalFrames[agent];
				agentsTotalFrames[agent] = newTotalFrames;
			}
			
			Logger::log("Remaining time: " + std::to_string(_lastTimeVisible) + "s");
		}
		else {
			++_fps;
		}

		std::vector<Team*> winners;
		GameState state = checkWinLoseConditions(winners);

		if (state == GameState::IN_PROGRESS) {
			
			for (Trigger* trigger : _gameMap->getTriggers()) {
				trigger->update(_gameTime);
			}

			if (Config.MultithreadingEnabled) {
				_updateHolder.notify_all();
			}
			else {
				for (Agent* agent : _agents) {
					agent->update(_gameTime);
				}
			}

			_missileManager->update(_gameTime);

			while (!_threadsToDispose.empty()) {
				Agent* agent = _threadsToDispose.front();
				size_t idx = common::indexOf(_agents, agent);
				size_t n = _agents.size();
				if (idx != n) {
					common::swapLastAndRemove(_agents, idx);
					agent->_thread.join();
					_threadsToDispose.pop();
					for (auto sender : agent->getNotificationSenders()) {
						sender->removeNotificationListener(agent);
					}
					for (auto listener : agent->getNotificationListeners()) {
						listener->removeNotificationSender(agent);
					}
					delete agent;
				}
			}
		}
		else if (state == GameState::ENDED_WIN) {
			std::cout << "Team " << winners.at(0)->getNumber() << " won!\n";
			_isUpdateEnabled = false;
		}
		else if (state == GameState::TIME_OUT) {
			if (winners.size() == 1) {
				std::cout << "Time's out. Team " << winners.at(0)->getNumber() << " won.\n";
			}
			else {
				String message = "Time's out. It's a draw between teams: ";
				bool addComma = false;
				for (Team* team : winners) {
					if (addComma) { message += ", "; }
					else { addComma = true; }
					message += std::to_string(team->getNumber());
				}
				message += ".\n";
				std::cout << message;
			} 
			_isUpdateEnabled = false;
		}
		else if (state == GameState::ENDED_DRAW) {
			std::cout << "Draw: all teams were eliminated!";
			_isUpdateEnabled = false;
		}		

		Logger::printLogs();
		Logger::clear();
	}
}

String toTimeString(size_t time) {
	return time < 10 ? "0" + std::to_string(time) : std::to_string(time);
}

void Game::render() const {

	Actor* currentActor = _playerAgent != nullptr ? _playerAgent->getActor() : nullptr;
	if (currentActor != nullptr && currentActor->isDead()) { currentActor = nullptr; }
	
	SDL_SetRenderDrawColor(_renderer, 128, 144, 192, 255);
	SDL_RenderClear(_renderer);

	Vector2 center = Vector2(Config.DisplayWidth / 2, Config.DisplayHeight / 2);
	Vector2 mousePos = Vector2(mousePosX, mousePosY);
	Segment centerMouseSegment = Segment(center, mousePos);
	
#ifdef _DEBUG

	if (_areAabbsVisible) {
		if (Config.CollisionResolver == "RegularGrid") {

			int gridSize = Config.RegularGridSize;
			auto grid = ((const RegularGrid*)_gameMap->getCollisionResolver());

			size_t n = (int)ceil(_gameMap->getWidth() / gridSize);
			size_t m = (int)ceil(_gameMap->getHeight() / gridSize);

			for (size_t i = 0; i < n; ++i) {
				for (size_t j = 0; j < m; ++j) {
					const RegularGrid::Region* region = grid->getRegionById(i, j);

					float left = i * gridSize;
					float right = left + gridSize;
					float top = j * gridSize;
					float bottom = top + gridSize;

					bool vld = true;
					for (DynamicEntity* a : region->dynamicObjects) {
						Vector2 p = a->getPosition();
						float r = a->getRadius();
						if (common::sqDist(p, left, right, top, bottom) > r*r) {
							vld = false;
							break;
						}
					}

					drawAabb(_renderer, Aabb(i * gridSize, j * gridSize, gridSize, gridSize), *_camera, colors::gray);
					drawString(_renderer, std::to_string(region->dynamicObjects.size()).c_str(), i * gridSize + 20, j * gridSize + 20, *_camera, Relative, false, vld ? colors::white : colors::red);
				}
			}
		}
		else if (Config.CollisionResolver == "AabbTree") {
			auto tree = (TreeCollisionResolver*)_gameMap->getCollisionResolver();

			for (Aabb& aabb : tree->getStaticAabbs()) {
				drawAabb(_renderer, aabb, *_camera, colors::pink);
			}

			for (Aabb& aabb : tree->getDynamicAabbs()) {
				drawAabb(_renderer, aabb, *_camera, colors::white);
			}
		}
	}
	
	if (_isNavigationMeshVisible) {
		for (Segment arc : _gameMap->getNavigationArcs()) {
			drawSegment(_renderer, arc, *_camera, colors::blue);
		}
		for (Vector2 point : _gameMap->getNavigationNodes()) {
			drawPoint(_renderer, point, *_camera, colors::blue);
		}
	}
	
	//drawPoint(_gameMap->getClosest(mousePos), colors::pink);

	/*if (_areAabbsVisible) {
		for (Aabb& aabb : _gameMap->getAabbs()) {
			drawAabb(aabb, colors::white);
		}
	}*/

	//for (Aabb& aabb : _gameMap->getRegionsContaining(common::Circle(mousePos, 20))) {
	//	drawAabb(aabb, colors::red);
	//}
	//drawCircle(mousePos, 20, colors::darkRed);

	/*if (_iscurrentPathVisible && currentActor != nullptr) {
		auto pathCopy = std::queue<Vector2>(currentActor->getCurrentPath());
		if (pathCopy.size() > 0) {
			Vector2 prev = currentActor->getPosition();
			drawPoint(prev, colors::yellow);
			while (!pathCopy.empty()) {
				Vector2 next = pathCopy.front();
				pathCopy.pop();
				drawSegment(Segment(prev, next), colors::yellow);
				drawPoint(next, colors::yellow);
				prev = next;
			}
		}
	}*/

#endif

	/*
	Vector2 wallCollision;
	if (_gameMap->raycastStatic(centerMouseSegment, wallCollision)) {
		drawPoint(wallCollision, colors::yellow);
	}
	*/

	/*
	// BARDZO SPOWALNIA PRACÊ PROGRAMU
	for (Wall wall : walls) {
		Segment wallSegment = wall.getSegment();
		drawString(std::to_string(wall.getId()).c_str(), 
			(wallSegment.from.x + wallSegment.to.x) / 2, 
			(wallSegment.from.y + wallSegment.to.y) / 2,
			colors::white);
	}
	*/

	auto walls = _gameMap->getWalls();
	for (StaticEntity* wall : walls) {
		//drawSegment(common::extendSegment(wall, Aabb(0, 0, DisplayWidth, DisplayHeight)), gray);
		for (Segment segment : wall->getBounds()) {
			drawSegment(_renderer, segment, *_camera, colors::black);
		}
	}

	for (Trigger* trigger : _gameMap->getTriggers()) {
		drawTrigger(_renderer, *trigger, *_camera);
	}

	for (Actor* actor : _gameMap->getActors()) {	
		drawActor(_renderer, *actor, *_camera, _playerAgent != nullptr && _playerAgent->getActor() == actor, _areHealthBarsVisible);
	}

//#ifdef _DEBUG
//	if (currentActor != nullptr) {
//
//		if (_iscurrentPathVisible) {
//			//auto viewBorders = currentActor->getViewBorders();
//			Vector2 pos = currentActor->getPosition();
//			//drawSegment(Segment(pos, pos + viewBorders.first * Config.ActorSightRadius), colors::pink);
//			//drawSegment(Segment(pos, pos + viewBorders.second * Config.ActorSightRadius), colors::pink);
//
//			auto vos = currentActor->getVelocityObstacles(currentActor->getObjectsInViewAngle());
//			for (VelocityObstacle vo : vos) {
//				fillRing(vo.obstacle->getPosition(), 24, 25, colors::yellow);
//				drawSegment(Segment(vo.apex, vo.apex + vo.side1 * Config.ActorSightRadius), colors::yellow);
//				drawSegment(Segment(vo.apex, vo.apex + vo.side2 * Config.ActorSightRadius), colors::yellow);
//			}
//
//			for (auto candidate : currentActor->computeCandidates(vos)) {
//				drawSegment(Segment(pos, pos + candidate.velocity * 50), colors::darkRed);
//			}
//	
//			auto walls = currentActor->getWallsNearGoal();
//			for (auto wall : walls) {
//				drawSegment(wall->getSegment(), colors::pink);
//			}
//
//			drawSegment(Segment(currentActor->getPosition(), currentActor->getLongGoal()), colors::cyan);
//			drawPoint(currentActor->getShortGoal(), colors::yellow);
//			drawPoint(currentActor->getLongGoal(), colors::cyan);
//		}
//	}
//
//#endif // _DEBUG

	//auto time = SDL_GetPerformanceCounter();

	SDL_Color missileColor;
	for (Missile& missile : _missileManager->getMissiles()) {		
		drawSegment(_renderer, Segment(missile.frontPosition, missile.backPosition), *_camera, getWeaponInfo(missile.weaponType).color);
	}

	for (common::Ring& ring : _missileManager->getExplosions(_gameTime)) {
		fillRing(_renderer, ring.center, ring.radius1, ring.radius2, *_camera, colors::red);
	}
	
	size_t seconds = getRemainingTime();
	size_t minutes = seconds / 60;
	seconds %= 60;
	drawString(_renderer, (toTimeString(minutes) + ":" + toTimeString(seconds)).c_str(), 
		Config.DisplayWidth / 2, Config.TimerPosition, *_camera, Absolute, true, colors::white);
	drawString(_renderer, ("FPS: " + std::to_string(_lastFps)).c_str(), 30 + Config.TimerPosition, 
		Config.TimerPosition, *_camera, Absolute, false, colors::white);

	SDL_RenderPresent(_renderer);
}

void Game::registerAgentToDispose(Agent* agent) {
	_threadsToDispose.push(agent);
	Actor* actor = agent->getActor();
	getMap()->remove(actor);
	if (actor->getTeam()->getRemainingActors() == 0) {
		std::cout << "Team " << actor->getTeam()->getNumber() << " was eliminated!\n";
	}
}

GameState Game::checkWinLoseConditions(std::vector<Team*>& winners) const {
	if (getRemainingTime() > 0) {
		size_t teamsRemaining = 0;
		size_t maxRemainingActors = 0;

		for (Team* team : _teams) {
			size_t remainingActors = team->getRemainingActors();
			if (remainingActors > 0) {
				++teamsRemaining;
			}
			if (remainingActors > maxRemainingActors) {
				maxRemainingActors = remainingActors;
				winners.clear();
				winners.push_back(team);
			}
			else if (remainingActors == maxRemainingActors) {
				winners.push_back(team);
			}
		}

		if (teamsRemaining > 1) {
			return GameState::IN_PROGRESS;
		}
		if (teamsRemaining == 1) {
			return GameState::ENDED_WIN;
		}
		return GameState::ENDED_DRAW;
	}
	else {
		float maxRemainingHealth = 0;

		for (Team* team : _teams) {
			float remainingHealth = team->getTotalRemainingHelath();
			if (remainingHealth > maxRemainingHealth) {
				maxRemainingHealth = remainingHealth;
				winners.clear();
				winners.push_back(team);
			}
			else if (remainingHealth == maxRemainingHealth) {
				winners.push_back(team);
			}
		}

		return GameState::TIME_OUT;
	}
}

GameTime Game::getRemainingTime() const { 
	return (_timeEnd - _gameTime) / SDL_GetPerformanceFrequency();
}
