#include <vector>
#include "Actor.h"
#include "actions/Action.h"
#include "engine/MissileManager.h"
#include "engine/CommonFunctions.h"
#include "main/Configuration.h"
#include "math/Math.h"
#include "main/Game.h"
#include "entities/Wall.h"
#include "actions/Die.h"

Actor::Actor(const String& name, const Vector2& position)
	: Movable(position) {
	_name = name;
	_health = common::min(Config.ActorMaxHealth, Config.ActorMaxHealth * Config.ActorInitialHealth);
	_armor = 0;
	_armorShotsRemaining = 0;	
	_currentWeapon = Config.DefaultWeapon;

	for (auto entry : MissileManager::getWeaponsInfo()) {
		WeaponState weaponState;
		weaponState.lastShot = 0;
		weaponState.state = WeaponLoadState::WEAPON_LOADED;
		weaponState.ammo = entry.second.initialAmmo;
		_weapons[entry.first] = weaponState;
	}
}

Actor::~Actor() { }

float Actor::getHealth() const { return _health; }

String Actor::getCurrentWeapon() const { return _currentWeapon; }

String Actor::getName() const { return _name; }

Team* Actor::getTeam() const { return _team; }

float Actor::getArmor() const { return _armor; }

int Actor::getRemainingArmorShots() const { return _armorShotsRemaining; }

bool Actor::isDead() const { return getCurrentActionType() == ActionType::DEAD; }

void Actor::setAmmo(const String& weaponName, int value) { _weapons[weaponName].ammo = value; }

void Actor::setArmor(float value) { _armor = value; }

void Actor::setRemainingArmorShots(int value) { _armorShotsRemaining = value; }

Action* Actor::getCurrentAction() const { return _currentAction; }

void Actor::clearCurrentAction() {
	if (_currentAction != nullptr) {
		delete _currentAction; 
		_currentAction = nullptr;
	}
}

bool Actor::setCurrentAction(Action* action) {
	if (_currentAction != nullptr) {
		if (_currentAction->getPriority() <= action->getPriority()) {
			_currentAction->finish(0);
			delete _currentAction;
			_currentAction = action;
			return true;
		}
		return false;
	}
	else {
		_currentAction = action;
		return true;
	}	
}

void Actor::setNextAction(Action* action) {
	if (_nextAction != nullptr) {
		delete _nextAction;
	}
	_nextAction = action;
}

float Actor::heal(float health) {
	if (isDead()) { return 0; }
	if (_health + health > Config.ActorMaxHealth) {
		health += _health - Config.ActorMaxHealth;
		_health = Config.ActorMaxHealth;
	}
	else {
		_health += health;
	}
	Logger::log("Actor " + _name + " restores " + std::to_string(health) + " health.");
	return health;
}

float Actor::getRadius() const { return Config.ActorRadius; }

bool Actor::isSolid() const { return true; }

void Actor::registerKill(const Destructible* destructible) {
	Team* team = destructible->getTeam();
	if (team != nullptr) {
		if (team != this->_team) { this->_kills++; }
		else { this->_friendkills++; }
		std::cout << destructible->getName() << " was killed by actor " << _name << ".\n";
	}
}

void Actor::setCurrentWeapon(const String& weaponName) {
	if (_currentWeapon != weaponName) {
		_currentWeapon = weaponName;
		Logger::log("Actor " + _name + " changed weapon to " + weaponName + ".");
	}
}

WeaponState& Actor::getWeaponState(const String& weaponName) { return _weapons[weaponName]; }

const WeaponState& Actor::getWeaponState(const String& weaponName) const { return _weapons.at(weaponName); }

bool Actor::updateCurrentAction(GameTime time) {
	if (_currentAction != nullptr) {
		if (!_currentAction->hasStarted()) {
			_currentAction->start(time);
		}
		if (_currentAction->update(time)) {
			_currentAction->finish(time);
			clearCurrentAction();
			if (_nextAction != nullptr) {
				_currentAction = _nextAction;
				_nextAction = nullptr;
			}
		}
		return true;
	}
	return false;
}

bool Actor::isLookingStraight() const {
	return _currentAction == nullptr || !_currentAction->locksRotation();
}

void Actor::update(GameTime time) {	
#ifdef _DEBUG	
	GameTime from, to;
	bool logIfSuccessful;
	   
	from = SDL_GetPerformanceCounter();
	logIfSuccessful = updateWeapons(time);
	to = SDL_GetPerformanceCounter();
	if (logIfSuccessful) {
		Logger::logIfNotIgnored("updateWeapons", "Update Weapons:         " + std::to_string(to - from));
	}

	from = SDL_GetPerformanceCounter();
	logIfSuccessful = updateCurrentAction(time);
	to = SDL_GetPerformanceCounter();
	if (logIfSuccessful) {
		Logger::logIfNotIgnored("updateCurrentAction", "Update Current Action:  " + std::to_string(to - from));
	}
#else
	updateWeapons(time);
	updateCurrentAction(time);
	updateMovement(time);
	updateOrientation(time);
#endif 

	Movable::update(time);
}

bool Actor::updateWeapons(GameTime time) {
	bool wasAntyhingUpdated = false;
	for (auto entry : MissileManager::getWeaponsInfo()) {
		const String& weaponName = entry.first;
		if (_weapons[weaponName].state == WeaponLoadState::WEAPON_UNLOADED
			&& time - _weapons[weaponName].lastShot
			> entry.second.reloadTime) {
			_weapons[weaponName].state = WeaponLoadState::WEAPON_LOADED;
			wasAntyhingUpdated = true;
		}
	}
	return wasAntyhingUpdated;
}

std::vector<Actor*> Actor::getSeenActors() const {
	std::vector<Actor*> result;
	for (Spottable* spottable : Spotter::getSpottedObjects()) {
		Actor* actor = dynamic_cast<Actor*>(spottable);
		if (actor != nullptr) {
			result.push_back(actor);
		}
	}
	return result;
}

std::vector<Trigger*> Actor::getSeenTriggers() const {
	std::vector<Trigger*> result;
	for (Spottable* spottable : Spotter::getSpottedObjects()) {
		Trigger* trigger = dynamic_cast<Trigger*>(spottable);
		if (trigger != nullptr) {
			result.push_back(trigger);
		}
	}
	return result;
}

float Actor::recieveDamage(float dmg) {
	if (dmg < _health) {
		_health -= dmg;
	}
	else {
		dmg = _health;
		_health = 0;
	}
	Logger::log("Actor " + _name + " receives " + std::to_string(dmg) + " damage.");
	return dmg;
}

bool Actor::isDestroyed() const {
	return _health <= 0;
}


void Actor::onDestroy() {
	// to do
	setCurrentAction(new DieAction(this));
	//throw;
}

float Actor::getSquareDistanceTo(const Vector2& point) const {
	return common::max(common::sqDist(_position, point) - common::sqr(getRadius()), 0);
}

float Actor::getMaxSpeed() const { return Config.ActorSpeed / Config.FPS; }

float Actor::getSightRadius() const { return Config.ActorSightRadius; }

Vector2 Actor::getPosition() const { return Movable::getPosition(); }

void Actor::onCollision(CollisionInvoker * actor, GameTime time) {
	std::cout << _name << " collided with " << dynamic_cast<Actor*>(actor)->getName() << ".\n";
}

ActionType Actor::getCurrentActionType() const {
	if (_currentAction != nullptr) {
		return _currentAction->getActionType();
	}
	return ActionType::IDLE;
}