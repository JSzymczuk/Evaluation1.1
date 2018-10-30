#pragma once

#include <vector>
#include "main/Configuration.h"
#include "math/Vector2.h"

class Actor;
class ActorInfo;
class TriggerInfo;
class Notification;
enum ActionType;

class ActorKnowledge {
public:
	ActorKnowledge(Actor* actor);
	~ActorKnowledge();

	ActorInfo getSelf() const;
	String getName() const;
	unsigned short getTeam() const;
	Vector2 getPosition() const;
	Vector2 getVelocity() const;
	Vector2 getShortDestination() const;
	Vector2 getLongDestination() const;
	ActionType getCurrentAction() const;
	bool canInterruptAction() const;
	bool isMoving() const;
	bool isWaiting() const;
	bool isDead() const;
	bool hasPositionChanged() const;
	float getOrientation() const;
	int getHealth() const;
	int getArmor() const;
	String getWeaponType() const;
	int getAmmo(const String& weaponName) const;
	bool isLoaded(const String& weaponName) const;
	float getEstimatedRemainingDistance() const;
	std::vector<ActorInfo> getSeenFriends() const;
	std::vector<ActorInfo> getSeenFoes() const;
	std::vector<ActorInfo> getSeenActors() const;
	std::vector<TriggerInfo> getSeenTriggers() const;

private:
	Actor* _actor;
};

