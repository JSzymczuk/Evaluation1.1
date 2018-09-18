#include "entities/Team.h"
#include "entities/Actor.h"
#include "agents/ObjectInfo.h"
#include "agents/ActorKnowledge.h"
#include "main/Game.h"

ActorKnowledge::ActorKnowledge(Actor* actor) : _actor(actor) {}
ActorKnowledge::~ActorKnowledge() {}

ActorInfo ActorKnowledge::getSelf() const { return ActorInfo(_actor, 0); }
String ActorKnowledge::getName() const { return _actor->getName(); }
unsigned short ActorKnowledge::getTeam() const { return _actor->getTeam()->getNumber(); }
Vector2 ActorKnowledge::getPosition() const { return _actor->getPosition(); }
bool ActorKnowledge::isMoving() const { return _actor->isMoving(); }
float ActorKnowledge::getOrientation() const { return _actor->getOrientation(); }
int ActorKnowledge::getHealth() const { return _actor->getHealth(); }
int ActorKnowledge::getArmor() const { return _actor->getArmor(); }
String ActorKnowledge::getWeaponType() const { return _actor->getCurrentWeapon(); }
int ActorKnowledge::getAmmo(const String& weaponName) const { return _actor->getWeaponState(weaponName).ammo; }
bool ActorKnowledge::isLoaded(const String& weaponName) const { return _actor->getWeaponState(weaponName).state == WeaponLoadState::WEAPON_LOADED; }
Vector2 ActorKnowledge::getVelocity() const { return _actor->getVelocity(); }
float ActorKnowledge::getEstimatedRemainingDistance() const { return _actor->estimateRemainingDistance(); }
Vector2 ActorKnowledge::getShortDestination() const { return _actor->getShortGoal(); }
Vector2 ActorKnowledge::getLongDestination() const { return _actor->getLongGoal(); }
bool ActorKnowledge::isDead() const { return _actor->isDead(); }
bool ActorKnowledge::hasPositionChanged() const { return _actor->hasPositionChanged(); }

std::vector<ActorInfo> ActorKnowledge::getSeenActors() const { 
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			result.push_back(ActorInfo((Actor*)seenObject, time));
		}
	}
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFriends() const {
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	Team* team = _actor->getTeam();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			Actor* other = (Actor*)seenObject;
			if (other->getTeam() == team) {
				result.push_back(ActorInfo(other, time));
			}
		}
	}
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFoes() const {
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	Team* team = _actor->getTeam();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			Actor* other = (Actor*)seenObject;
			if (other->getTeam() != team) {
				result.push_back(ActorInfo(other, time));
			}
		}
	}
	return result;
}

std::vector<TriggerInfo> ActorKnowledge::getSeenTriggers() const {
	std::vector<TriggerInfo> result;
	GameTime time = Game::getCurrentTime();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::TRIGGER) {
			Trigger* trigger = (Trigger*)seenObject;
			if (trigger->isActive()) {
				result.push_back(TriggerInfo(trigger, time));
			}
		}
	}
	return result;
}