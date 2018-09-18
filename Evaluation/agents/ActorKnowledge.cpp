#include "entities/Team.h"
#include "entities/Actor.h"
#include "agents/ActorInfo.h"
#include "agents/ActorKnowledge.h"

ActorKnowledge::ActorKnowledge(Actor* actor) : _actor(actor) {}
ActorKnowledge::~ActorKnowledge() {}

ActorInfo ActorKnowledge::getSelf() const { return ActorInfo(_actor, 0); }
String ActorKnowledge::getName() const { return _actor->getName(); }
unsigned short ActorKnowledge::getTeam() const { return _actor->getTeam()->getNumber(); }
Vector2 ActorKnowledge::getPosition() const { return _actor->getPosition(); }
Vector2 ActorKnowledge::getDestination() const { return _actor->getDestination(); }
bool ActorKnowledge::isMoving() const { return _actor->isMoving(); }
float ActorKnowledge::getOrientation() const { return _actor->getOrientation(); }
int ActorKnowledge::getHealth() const { return _actor->getHealth(); }
int ActorKnowledge::getArmor() const { return _actor->getArmor(); }
String ActorKnowledge::getWeaponType() const { return _actor->getCurrentWeapon(); }
int ActorKnowledge::getAmmo(const String& weaponName) const { return _actor->getWeaponState(weaponName).ammo; }
bool ActorKnowledge::isLoaded(const String& weaponName) const { return _actor->getWeaponState(weaponName).state == WeaponLoadState::WEAPON_LOADED; }


std::vector<ActorInfo> ActorKnowledge::getSeenActors() const { 
	std::vector<ActorInfo> result;
	/*for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			result.push_back(ActorInfo((Actor*)seenObject));
		}
	}*/
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFriends() const {
	std::vector<ActorInfo> result;
	/*Team* team = _actor->getTeam();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			Actor* other = (Actor*)seenObject;
			if (other->getTeam() == team) {
				result.push_back(ActorInfo(other));
			}
		}
	}*/
	return result;
}

std::vector<ActorInfo> ActorKnowledge::getSeenFoes() const {
	std::vector<ActorInfo> result;
	/*Team* team = _actor->getTeam();
	for (GameDynamicObject* seenObject : _actor->getSeenObjects()) {
		if (seenObject->getGameObjectType() == GameDynamicObjectType::ACTOR) {
			Actor* other = (Actor*)seenObject;
			if (other->getTeam() != team) {
				result.push_back(ActorInfo(other));
			}
		}
	}*/
	return result;
}


std::vector<TriggerInfo> ActorKnowledge::getSeenTriggers() const {
	std::vector<TriggerInfo> result;
	return result;
}