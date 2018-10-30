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
bool ActorKnowledge::isWaiting() const { return _actor->isWaiting(); }
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
ActionType ActorKnowledge::getCurrentAction() const { return _actor->getCurrentActionType(); }
bool ActorKnowledge::isDead() const { return _actor->isDead(); }
bool ActorKnowledge::hasPositionChanged() const { return _actor->hasPositionChanged(); }
bool ActorKnowledge::canInterruptAction() const { return !_actor->getCurrentAction()->isTransactional(); }

std::vector<ActorInfo> ActorKnowledge::getSeenActors() const { 
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	for (Actor* other : _actor->getSeenActors()) {
		result.push_back(ActorInfo(other, time));
	}
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFriends() const {
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	Team* team = _actor->getTeam();
	for (Actor* other : _actor->getSeenActors()) {
		if (other->getTeam() == team) {
			result.push_back(ActorInfo(other, time));
		}
	}	
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFoes() const {
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	Team* team = _actor->getTeam();
	for (Actor* other : _actor->getSeenActors()) {
		if (other->getTeam() != team) {
			result.push_back(ActorInfo(other, time));
		}
	}
	return result;
}

std::vector<TriggerInfo> ActorKnowledge::getSeenTriggers() const {
	std::vector<TriggerInfo> result;
	GameTime time = Game::getCurrentTime();
	for (Trigger* trigger : _actor->getSeenTriggers()) {
		if (trigger->isActive()) {
			result.push_back(TriggerInfo(trigger, time));
		}
	}
	return result;
}