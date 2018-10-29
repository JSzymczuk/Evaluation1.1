#pragma once

#include "main/Configuration.h"
#include "math/Math.h"
#include "entities/Entity.h"

class Destructible;

class MissileOwner {
public:	
	virtual Vector2 getPosition() const = 0;
	virtual String getCurrentWeapon() const = 0;
	virtual void registerKill(const Destructible* destructible) = 0;
};

class Missile : public Entity {
public:
	MissileOwner* owner;
	Vector2 origin;
	Vector2 target;
	String weaponType;
	GameTime timeFired;
	GameTime timeHit;
	bool isActive;
	bool isTargetReached;
	Vector2 frontPosition;
	Vector2 backPosition;
};
