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
#include "engine//CommonFunctions.h"

Game::Game() {
	if (_instance != nullptr) {
		delete _instance;
	}
	_instance = this;
}

Game::~Game() {
	for (Agent* a : _agents) {
		delete a;
	}

	GameMap::destroy(_gameMap);
	destroyLuaEnv(_luaEnv);

	if (_instance == this) { _instance = nullptr; }
}

Game* Game::_instance = nullptr;

Game* Game::getInstance() { return _instance; }

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

	initializeTeams(settings.actors);

	start(settings.duration);

	return true;
}

void Game::start(size_t durationInSeconds) {
	for (Agent* agent : _agents) {
		agent->start();
	}
	_gameTime = SDL_GetPerformanceCounter();
	_timeEnd = _gameTime + 10000000 * durationInSeconds;
	_lastTimeVisible = durationInSeconds;
}

void Game::initializeTeams(const std::vector<ActorLoadedData>& actorsData) {
	// Actor* actor = new Actor("test" + std::to_string(i), 0, center + Vector2(cosf(i * angle), sinf(i * angle)) * r);
	std::map<size_t, Team*> teams;
	std::map<Team*, std::vector<Agent*>> teamAgents;

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

		if (_gameMap->place(actor)) {
			if (actorData.script == Config.AgentControlled && _playerAgent == nullptr) {
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
		}
		else {
			delete actor;
		}
	}

	std::vector<SDL_Color> teamColors = { 
		{ 47, 63, 191 }, // niebieski
		{ 191, 31, 31 }, // czerwony
		{ 47, 143, 47 }, // zielony
		{ 191, 191, 47 }, // ¿ó³ty
		{ 160, 32, 208 }, // fioletowy
		{ 96, 180, 180 }, // turkusowy
		{ 96, 180, 180 }, // turkusowy
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
	SDL_PollEvent(&event);

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

		if (keyboardState[SDL_SCANCODE_SPACE]) {
			if (_isNavigationMeshVisible) {
				_isNavigationMeshVisible = false;
				_areAabbsVisible = false;
				_iscurrentPathVisible = false;
			}
			else {
				_isNavigationMeshVisible = true;
				_areAabbsVisible = true;
				_iscurrentPathVisible = true;
			}
		}

		if (keyboardState[SDL_SCANCODE_TAB]) {
			_isUpdateEnabled = !_isUpdateEnabled;
		}

		if (keyboardState[SDL_SCANCODE_LCTRL] && keyboardState[SDL_SCANCODE_L]) {
			if (Logger::isLogging()) { Logger::stopLogging(); }
			else { Logger::startLogging(); }			
		}
		break;
	}
	case SDL_QUIT:
		_isRunning = false;
		break;
	default: break;
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

void Game::update() {
	if (_isUpdateEnabled) {
		_gameTime = SDL_GetPerformanceCounter();
		if (_gameTime > _timeEnd) {
			_gameTime = _timeEnd;
		}

		size_t remainingTimeInSeconds = (size_t)(getRemainingTime() / 10000000);
		if (remainingTimeInSeconds < _lastTimeVisible) {
			_lastTimeVisible = remainingTimeInSeconds;
			Logger::log("Remaining time: " + std::to_string(_lastTimeVisible) + "s");
		}

		std::vector<Team*> winners;
		GameState state = checkWinLoseConditions(winners);

		if (state == GameState::IN_PROGRESS) {

			for (Trigger* trigger : _gameMap->getTriggers()) {
				trigger->update(_gameTime);
			}

			_updateHolder.notify_all();
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

void Game::render() {

	Actor* currentActor = _playerAgent != nullptr ? _playerAgent->getActor() : nullptr;
	if (currentActor != nullptr && currentActor->isDead()) { currentActor = nullptr; }
	
	SDL_SetRenderDrawColor(_renderer, 128, 144, 192, 255);
	SDL_RenderClear(_renderer);

	Vector2 center = Vector2(Config.DisplayWidth / 2, Config.DisplayHeight / 2);
	Vector2 mousePos = Vector2(mousePosX, mousePosY);
	Segment centerMouseSegment = Segment(center, mousePos);

	if (_isNavigationMeshVisible) {

		int gridSize = Config.RegularGridSize;

		size_t n = (int)ceil(_gameMap->getWidth() / gridSize);
		size_t m = (int)ceil(_gameMap->getHeight() / gridSize);
		for (size_t i = 0; i < n; ++i) {
			for (size_t j = 0; j < m; ++j) {
				drawAabb(Aabb(i * gridSize, j * gridSize, gridSize, gridSize), colors::gray);
			}
		}
	}

	auto walls = _gameMap->getWalls();
	for (GameStaticObject* wall : walls) {
		//drawSegment(common::extendSegment(wall, Aabb(0, 0, DisplayWidth, DisplayHeight)), gray);
		for (Segment segment : wall->getBounds()) {
			drawSegment(segment, colors::black);
		}
	}

#ifdef _DEBUG
	
	if (_isNavigationMeshVisible) {
		for (Segment arc : _gameMap->getNavigationArcs()) {
			drawSegment(arc, colors::blue);
		}
		for (Vector2 point : _gameMap->getNavigationNodes()) {
			drawPoint(point, colors::blue);
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

	if (_iscurrentPathVisible && currentActor != nullptr) {
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
	}

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

	int actorRadius = Config.ActorRadius;

	for (Trigger* trigger : _gameMap->getTriggers()) {
		drawTrigger(*trigger);
	}

	for (Actor* actor : _gameMap->getActors()) {		
		if (actor->isMoving()) {
			//GameTime from, to;
			//from = SDL_GetPerformanceCounter();
			drawActor(*actor);
			//to = SDL_GetPerformanceCounter();
			//Logger::log("Render:                 " + std::to_string(to - from));
		}
		else {
			drawActor(*actor);
		}
#ifdef _DEBUG
		if (_areAabbsVisible) {
			drawCircle(actor->getPosition(), actorRadius, colors::black);
		}
#endif // _DEBUG
	}

#ifdef _DEBUG
	if (currentActor != nullptr) {

		if (_iscurrentPathVisible) {
			auto viewBorders = currentActor->getViewBorders();
			Vector2 pos = currentActor->getPosition();
			drawSegment(Segment(pos, pos + viewBorders.first * Config.ActorSightRadius), colors::pink);
			drawSegment(Segment(pos, pos + viewBorders.second * Config.ActorSightRadius), colors::pink);

			auto vos = currentActor->getVelocityObstacles(currentActor->getObjectsInViewAngle());
			for (VelocityObstacle vo : vos) {
				fillRing(vo.obstacle->getPosition(), 24, 25, colors::yellow);
				drawSegment(Segment(vo.apex, vo.apex + vo.side1 * Config.ActorSightRadius), colors::yellow);
				drawSegment(Segment(vo.apex, vo.apex + vo.side2 * Config.ActorSightRadius), colors::yellow);
			}

			for (auto candidate : currentActor->computeCandidates(vos)) {
				drawSegment(Segment(pos, pos + candidate.velocity * 50), colors::darkRed);
			}
	
			auto walls = currentActor->getWallsNearGoal();
			for (auto wall : walls) {
				drawSegment(wall->getSegment(), colors::pink);
			}

			drawSegment(Segment(currentActor->getPosition(), currentActor->getLongGoal()), colors::cyan);
			drawPoint(currentActor->getShortGoal(), colors::yellow);
			drawPoint(currentActor->getLongGoal(), colors::cyan);
		}
	}

#endif // _DEBUG

	auto time = SDL_GetPerformanceCounter();

	SDL_Color missileColor;
	for (Missile& missile : _missileManager->getMissiles()) {		
		drawSegment(Segment(missile.frontPosition, missile.backPosition), getWeaponInfo(missile.weaponType).color);
	}

	for (common::Ring& ring : _missileManager->getExplosions(time)) {
		fillRing(ring.center, ring.radius1, ring.radius2, colors::red);
	}

	size_t seconds = (size_t)(getRemainingTime() / 10000000);
	size_t minutes = seconds / 60;
	seconds %= 60;
	drawString((toTimeString(minutes) + ":" + toTimeString(seconds)).c_str(), Config.DisplayWidth / 2, Config.TimerPosition, colors::white);

	SDL_RenderPresent(_renderer);
}

void Game::drawTexture(SDL_Texture* texture, const Vector2& position) const {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	SDL_Rect tRect = { int(position.x), int(position.y), width, height };
	SDL_RenderCopy(_renderer, texture, nullptr, &tRect);
}

void Game::drawTexture(SDL_Texture* texture, const Vector2& center, float orientation, bool horizontalFlip) const {
	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	int widthHalf = width / 2;
	int heightHalf = height / 2;
	SDL_Rect tRect = { int(center.x) - widthHalf, int(center.y) - heightHalf, width, height };
	SDL_Point tCenter = { widthHalf, heightHalf };
	SDL_RenderCopyEx(_renderer, texture, nullptr, &tRect, common::degrees(orientation), &tCenter, 
		horizontalFlip ? SDL_RendererFlip::SDL_FLIP_HORIZONTAL : SDL_RendererFlip::SDL_FLIP_NONE);
}

void Game::drawString(const char* string, int x, int y, const SDL_Color& color) const {
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(ResourceManager::get()->getFont("mainfont"), string, color);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(_renderer, surfaceMessage);
	int w = surfaceMessage->w, h = surfaceMessage->h;
	SDL_Rect Message_rect = { x - w / 2, y - h / 2, w, h};
	SDL_RenderCopy(_renderer, Message, nullptr, &Message_rect); 
	SDL_DestroyTexture(Message);
	SDL_FreeSurface(surfaceMessage);
}

void Game::drawAabb(const Aabb& aabb, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect{ int(aabb.getLeft()), int(aabb.getTop()), int(aabb.getWidth()), int(aabb.getHeight()) };
	SDL_RenderDrawRect(_renderer, &rect);
}

void Game::fillAabb(const Aabb& aabb, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect{ int(aabb.getLeft()), int(aabb.getTop()), int(aabb.getWidth()), int(aabb.getHeight()) };
	SDL_RenderFillRect(_renderer, &rect);
}

void Game::drawSegment(const Segment& segment, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(_renderer, int(segment.from.x), int(segment.from.y), int(segment.to.x), int(segment.to.y));
}

void Game::drawPoint(const Vector2& point, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	SDL_Rect rect = { int(point.x - 2), int(point.y - 2), 5, 5 };
	SDL_RenderFillRect(_renderer, &rect);
}

void Game::drawCircle(const Vector2& center, float radius, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x), y = int(center.y), r = int(radius), r2 = r * r;
	for (int i = -r; i <= r; ++i) {
		int t = sqrt(r2 - i * i);
		SDL_RenderDrawPoint(_renderer, x - t, y + i);
		SDL_RenderDrawPoint(_renderer, x + t, y + i);
	}
}

void Game::fillCircle(const Vector2& center, float radius, const SDL_Color& color) const {
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x), y = int(center.y), r = int(radius), r2 = r * r;
	for (int i = -r; i <= r; ++i) {
		int t = sqrt(r2 - i * i);
		SDL_RenderDrawLine(_renderer, x - t, y + i, x + t, y + i);
	}
}

void Game::fillRing(const Vector2& center, float radius1, float radius2, const SDL_Color& color) const {
	if (radius1 > radius2) { common::swap(radius1, radius2); }
	SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, color.a);
	int x = int(center.x), y = int(center.y), r1 = int(radius1), r12 = r1 * r1, r2 = int(radius2), r22 = r2 * r2;
	for (int i = -r2; i <= r2; ++i) {
		if (-r1 <= i && i <= r1) {
			int t1 = sqrt(r12 - i * i);
			int t2 = sqrt(r22 - i * i);
			SDL_RenderDrawLine(_renderer, x - t2, y + i, x - t1, y + i);
			SDL_RenderDrawLine(_renderer, x + t1, y + i, x + t2, y + i);
		}
		else {
			int t = sqrt(r22 - i * i);
			SDL_RenderDrawLine(_renderer, x - t, y + i, x + t, y + i);
		}
	}
}

void Game::drawTrigger(const Trigger& trigger) const {
	try {
		if (trigger.isActive()) {
			SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(Config.TriggerRingTextureKey));
			drawTexture(texture, trigger.getPosition(), trigger.getOrientation(), false);
			SDL_DestroyTexture(texture);
		}
	}
	catch (...) { std::cout << "Error" << std::endl; }
}

SDL_Color blendColors(const SDL_Color& c1, const SDL_Color& c2, float t) {
	Uint8 r = t * c1.r + (1 - t) * c2.r;
	Uint8 g = t * c1.g + (1 - t) * c2.g;
	Uint8 b = t * c1.b + (1 - t) * c2.b;
	Uint8 a = t * c1.a + (1 - t) * c2.a;
	return { r, g, b, a };
}

void Game::drawActor(const Actor& actor) const {
	try {
		if (!actor.isDead()) {
			SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(Config.ActorRingTextureKey));
			Vector2 actorPos = actor.getPosition();
			fillCircle(actorPos, Config.ActorRadius, actor.getTeam()->getColor());

			if (_playerAgent->getActor() == &actor) {
				fillRing(actorPos, Config.ActorSelectionRing, Config.ActorSelectionRing + 1, colors::green);
			}

			drawTexture(texture, actorPos, actor.getOrientation(), false);
			SDL_DestroyTexture(texture);

			if (_areHealthBarsVisible) {
				SDL_Rect rect = { 
					actorPos.x - Config.HealthBarWidth / 2, 
					actorPos.y + Config.ActorRadius + Config.HealthBarPosition, 
					Config.HealthBarWidth, 
					Config.HealthBarHeight 
				};
				SDL_SetRenderDrawColor(_renderer, 
					Config.HealthBarBackColor.r, 
					Config.HealthBarBackColor.g, 
					Config.HealthBarBackColor.b, 255
				);
				SDL_RenderFillRect(_renderer, &rect);

				--rect.x; --rect.y;
				rect.w += 2; rect.h += 2;
				SDL_SetRenderDrawColor(_renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(_renderer, &rect);

				drawString(actor.getName().c_str(), actorPos.x, actorPos.y + Config.ActorNamePosition, colors::white);

				++rect.x; ++rect.y;
				rect.w -= 2; rect.h -= 2;
				float hpPerc = common::clamp((float)actor.getHealth() / Config.ActorMaxHealth, 0, 1);
				SDL_Color color = hpPerc > 0.5f ? blendColors(
						Config.HealthBarFullColor, 
						Config.HealthBarHalfColor, 
						(hpPerc - 0.5f) / 0.5f)
					: blendColors(
						Config.HealthBarHalfColor,
						Config.HealthBarEmptyColor, 
						hpPerc / 0.5f);
				SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
				rect.w *= hpPerc;
				SDL_RenderFillRect(_renderer, &rect);


				//texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(Config.HealthBarTextureKey));
				//drawTexture(texture, Vector2(actorPos.x, actorPos.y + Config.ActorRadius + 15 + Config.HealthBarHeight / 2), 0, false);
				//SDL_DestroyTexture(texture);
			}
		}
	}
	catch (...) { std::cout << "Error" << std::endl; }
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
	GameTime remaining = _timeEnd - _gameTime;
	return remaining > 0 ? remaining : 0;
}
