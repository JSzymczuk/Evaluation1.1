#pragma once

#include "entities/Actor.h"

class ActorInfo {
public:
	const char* getName() const;
	unsigned short getTeam() const;
	Vector2 getPosition() const;
	float getOrientation() const;
	int getHealth() const;
	int getArmor() const;
	String getWeaponType() const;

	ActorInfo(Actor* actor);
	~ActorInfo();
	
private:
	String _name;
	String _weapon;
	unsigned short _team;
	Vector2 _position;
	float _orientation;
	int _health;
	int _armor;
};
