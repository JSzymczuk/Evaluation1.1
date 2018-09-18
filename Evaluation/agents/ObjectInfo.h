#pragma once

#include "main/Configuration.h"
#include "math/Vector2.h"

class Actor;
class Trigger;


class ObjectInfo {
public:
	ObjectInfo(GameTime time);
	virtual ~ObjectInfo();
	GameTime getObservationTime() const;

private:
	GameTime _time;
};


class TriggerInfo : public ObjectInfo {
public:
	TriggerInfo(Trigger* trigger, GameTime time);
	~TriggerInfo();

	String getName() const;
	Vector2 getPosition() const;
	bool isActive() const;

private:
	String _name;
	Vector2 _position;
	bool _isActive;
};


class ActorInfo : public ObjectInfo {
public:
	ActorInfo(Actor* actor, GameTime time);
	~ActorInfo();

	String getName() const;
	unsigned short getTeam() const;
	Vector2 getPosition() const;
	float getOrientation() const;
	int getHealth() const;
	int getArmor() const;
	String getWeaponType() const;
	bool isDead() const;
	
private:
	String _name;
	String _weapon;
	unsigned short _team;
	Vector2 _position;
	float _orientation;
	int _health;
	int _armor;
};
