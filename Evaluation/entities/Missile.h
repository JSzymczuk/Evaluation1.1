#pragma once

#include "main/Configuration.h"
#include "math/Math.h"

class Actor;

struct Missile {
	Actor* owner;
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
