#include "Game.h"
#include "engine/ResourceManager.h"
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

Game::Game() {
	if (_instance != nullptr) {
		delete _instance;
	}
	_instance = this;
}

Game::~Game() {
	if (_instance == this) { _instance = nullptr; }
}

Game* Game::_instance = nullptr;

Game* Game::getInstance() { return _instance; }

MissileManager* Game::getMissileManager() const { return _missileManager; }

GameMap* Game::getMap() const { return _gameMap; }

std::vector<Team*> Game::getTeams() const { return _instance->_teams; }

std::vector<Actor*> Game::getActors() const {
	std::vector<Actor*> result;
	if (_instance != nullptr) {
		for (GameDynamicObject* entity : _instance->_gameMap->getEntities()) {
			if (entity->getGameObjectType() == GameDynamicObjectType::ACTOR) {
				result.push_back((Actor*)entity);
			}
		}
	}
	return result;
}
std::vector<Trigger*> Game::getTriggers() const {
	std::vector<Trigger*> result;
	if (_instance != nullptr) {
		for (GameDynamicObject* entity : _instance->_gameMap->getEntities()) {
			if (entity->getGameObjectType() == GameDynamicObjectType::TRIGGER) {
				result.push_back((Trigger*)entity);
			}
		}
	}
	return result;
}

void Game::trySelectActor(const Vector2& point) {
	_actor = nullptr;
	for (GameDynamicObject* entity : _gameMap->checkCollision(point)) {
		if (entity->getGameObjectType() == GameDynamicObjectType::ACTOR && entity->getCollisionArea().contains(point)) {
			_actor = (Actor*)entity;
			break;
		}
	}
}

bool Game::initialize(const char* title, int width, int height) {
	if (!SDL_Init(SDL_INIT_EVERYTHING)) {
		_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
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

	if (!ResourceManager::initialize()) { return false; }
	TriggerFactory::initialize();

	auto rm = ResourceManager::get();
	rm->loadFont("mainfont", "content/CourierNew.ttf", 14);
	rm->loadImage(ActorRingTextureKey, ActorRingTexturePath);
	rm->loadImage(TriggerRingTextureKey, TriggerRingTexturePath);
	rm->loadImage(HealthBarTextureKey, HealthBarTexturePath);

	_actor = nullptr;
	_gameMap = GameMap::create("test.map");

	_missileManager = new MissileManager();
	_missileManager->initialize(_gameMap);

	std::vector<GameDynamicObject*> entities;
	float w3 = DisplayWidth / 3, h3 = DisplayHeight / 3;
	entities.push_back(new Actor("Actor1", 0, Vector2(DisplayWidth / 2, DisplayHeight / 2)));
	entities.push_back(new Actor("Actor2", 0, Vector2(w3, h3)));
	entities.push_back(new Actor("Actor3", 0, Vector2(w3, 2 * h3)));
	entities.push_back(new Actor("Actor4", 0, Vector2(2 * w3, h3)));
	entities.push_back(new Actor("Actor5", 0, Vector2(2 * w3, 2 * h3)));

	entities.push_back(TriggerFactory::create(TriggerType::HEALTH, Vector2(100, 100)));
	entities.push_back(TriggerFactory::create("Railgun", Vector2(DisplayWidth - 100, 100)));
	entities.push_back(TriggerFactory::create("Chaingun", Vector2(100, DisplayHeight - 100)));
	entities.push_back(TriggerFactory::create(TriggerType::ARMOR, Vector2(DisplayWidth - 100, DisplayHeight - 100)));

	for (auto invalidEntity : _gameMap->initializeEntities(entities)) {
		delete invalidEntity;
	}
		
	return true;
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
			Actor* prevActor = _actor;
			if (_actor == nullptr || keyboardState[SDL_SCANCODE_LCTRL]) {
				trySelectActor(Vector2(mousePosX, mousePosY));
				if (_actor == nullptr && prevActor != nullptr && keyboardState[SDL_SCANCODE_LALT]) {
					Action* action = new WanderAction(prevActor);
					if (!prevActor->setCurrentAction(action)) {
						Logger::log("Failed");
						delete action;
					}
				}
			}
			else if (_actor != nullptr) {
				Action* action = new ShootAction(_actor, Vector2(mousePosX, mousePosY));
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT) {
			if (_actor != nullptr) {
				Action* action;
				if (keyboardState[SDL_SCANCODE_LSHIFT]) {
					action = new MoveAtAction(_actor, Vector2(mousePosX, mousePosY) - _actor->getPosition());
				}
				else if (keyboardState[SDL_SCANCODE_LALT]) {
					action = new FaceAction(_actor, Vector2(mousePosX, mousePosY));
				}
				else {
					action = new MoveAction(_actor, Vector2(mousePosX, mousePosY));
				}
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
				//_path = _gameMap->findPath(_actor->getPosition(), Vector2(mousePosX, mousePosY));
				//_actor->move(_path);
			}
			//actor->lookAt(Vector2(mousePosX, mousePosY));
			//actor->move(Vector2(mousePosX, mousePosY));
		}
		break;
	case SDL_KEYDOWN: {
		const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
		if (_actor != nullptr) {
			if (keyboardState[SDL_SCANCODE_1]) { 
				Action* action = new ChangeWeaponAction(_actor, "Shotgun");
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
			}
			else if (keyboardState[SDL_SCANCODE_2]) {
				Action* action = new ChangeWeaponAction(_actor, "Railgun");
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
			}
			else if (keyboardState[SDL_SCANCODE_3]) {
				Action* action = new ChangeWeaponAction(_actor, "Chaingun");
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
			}
			else if (keyboardState[SDL_SCANCODE_4]) {
				Action* action = new ChangeWeaponAction(_actor, "RocketLauncher");
				if (!_actor->setCurrentAction(action)) {
					Logger::log("Failed");
					delete action;
				}
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
		break;
	}
	/*case SDL_KEYUP: {
		const Uint8* keyboardState = SDL_GetKeyboardState(nullptr);
		if (!keyboardState[SDL_SCANCODE_SPACE]) {
			_areAabbsVisible = true;
			_areHealthBarsVisible = true;
			_isNavigationMeshVisible = true;
			_iscurrentPathVisible = true;
		}
		break;
	}*/
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

	GameTime time = SDL_GetPerformanceCounter();

	for (GameDynamicObject* entity : _gameMap->getEntities()) {
		entity->update(time);
	}
	_missileManager->update(time);

	Logger::printLogs();
	Logger::clear();
}

void Game::render() {
	
	SDL_SetRenderDrawColor(_renderer, 128, 144, 192, 255);
	SDL_RenderClear(_renderer);

	Vector2 center = Vector2(DisplayWidth / 2, DisplayHeight / 2);
	Vector2 mousePos = Vector2(mousePosX, mousePosY);
	Segment centerMouseSegment = Segment(center, mousePos);
	
	auto walls = _gameMap->getWalls();
	for (Wall wall : walls) {
		//drawSegment(common::extendSegment(wall, Aabb(0, 0, DisplayWidth, DisplayHeight)), gray);
		drawSegment(wall.getSegment(), colors::black);
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

	if (_areAabbsVisible) {
		for (Aabb& aabb : _gameMap->getAabbs()) {
			drawAabb(aabb, colors::white);
		}
	}

	if (_iscurrentPathVisible && _actor != nullptr) {
		auto pathCopy = std::queue<Vector2>(_actor->getCurrentPath());
		if (pathCopy.size() > 0) {
			Vector2 prev = _actor->getPosition();
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
	// BARDZO SPOWALNIA PRAC� PROGRAMU
	for (Wall wall : walls) {
		Segment wallSegment = wall.getSegment();
		drawString(std::to_string(wall.getId()).c_str(), 
			(wallSegment.from.x + wallSegment.to.x) / 2, 
			(wallSegment.from.y + wallSegment.to.y) / 2,
			colors::white);
	}
	*/

	if (_actor != nullptr) { 
		fillRing(_actor->getPosition(), 24, 25, colors::green); 

		for (Actor* seenActor : _actor->getSeenActors()) {
			fillRing(seenActor->getPosition(), 24, 24, colors::cyan);
		}
	}

	for (GameDynamicObject* entity : _gameMap->getEntities()) {
		switch (entity->getGameObjectType()) {
		case GameDynamicObjectType::ACTOR:
			drawActor(*((Actor*)entity));
			break;
		case GameDynamicObjectType::TRIGGER:
			drawTrigger(*((Trigger*)entity));
			break;
		}
	}

#ifdef _DEBUG
//	if (_actor != nullptr) {
//		auto viewBorders = _actor->getViewBorders();
//		Vector2 pos = _actor->getPosition();
//		drawSegment(Segment(pos, pos + viewBorders.first * ActorSightRadius), colors::pink);
//		drawSegment(Segment(pos, pos + viewBorders.second * ActorSightRadius), colors::pink);
//
//		auto vos = _actor->getVelocityObstacles(_actor->getActorsInViewAngle());
//		for (VelocityObstacle vo : vos) {
//			fillRing(vo.obstacle->getPosition(), 24, 25, colors::yellow);
//			drawSegment(Segment(vo.apex, vo.apex + vo.side1 * ActorSightRadius), colors::yellow);
//			drawSegment(Segment(vo.apex, vo.apex + vo.side2 * ActorSightRadius), colors::yellow);
//		}
//
//		for (auto candidate : _actor->computeCandidates(vos)) {
//			drawSegment(Segment(pos, pos + candidate.velocity * 50), colors::darkRed);
//		}
//	}
//
	if (_actor != nullptr) {
		auto walls = _actor->getWallsNearGoal();
		for (const Wall& wall : walls) {
			drawSegment(wall.getSegment(), colors::pink);
		}
		//drawPoint(_actor->getNextSafeGoal(), colors::yellow);
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
	SDL_Rect Message_rect = { x, y, surfaceMessage->w, surfaceMessage->h };
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
			SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(TriggerRingTextureKey));
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
			SDL_Texture* texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(ActorRingTextureKey));
			Vector2 actorPos = actor.getPosition();
			drawTexture(texture, actorPos, actor.getOrientation(), false);
			SDL_DestroyTexture(texture);

			if (_areHealthBarsVisible) {
				SDL_Rect rect = { actorPos.x - HealthBarWidth / 2, actorPos.y + ActorRadius + 15, HealthBarWidth, HealthBarHeight };
				SDL_SetRenderDrawColor(_renderer, HealthBarBackColor.r, HealthBarBackColor.g, HealthBarBackColor.b, 255);
				SDL_RenderFillRect(_renderer, &rect);
				float hpPerc = common::clamp((float)actor.getHealth() / ActorMaxHealth, 0, 1);
				SDL_Color color = hpPerc > 0.5f ? blendColors(HealthBarFullColor, HealthBarHalfColor, (hpPerc - 0.5f) / 0.5f)
					: blendColors(HealthBarHalfColor, HealthBarEmptyColor, hpPerc / 0.5f);
				SDL_SetRenderDrawColor(_renderer, color.r, color.g, color.b, 255);
				rect.w *= hpPerc;
				SDL_RenderFillRect(_renderer, &rect);

				texture = SDL_CreateTextureFromSurface(_renderer, ResourceManager::get()->getImage(HealthBarTextureKey));
				drawTexture(texture, Vector2(actorPos.x, actorPos.y + ActorRadius + 15 + HealthBarHeight / 2), 0, false);
				SDL_DestroyTexture(texture);
			}
		}
	}
	catch (...) { std::cout << "Error" << std::endl; }
}
