#pragma once

#include "Entity.h"

enum TriggerType {
	HEALTH,
	WEAPON,
	ARMOR,

	TRIGGERS_SIZE
};

class Actor;

class Trigger : public GameDynamicObject {
public:
	Trigger(const Vector2& position, const String& label);
	~Trigger();

	bool isSolid() const;
	bool isStaticElement() const;
	GameDynamicObjectType getGameObjectType() const;
	Aabb getAabb() const;
	float getRadius() const;
	
	bool isActive() const;
	void update(GameTime time);

	virtual TriggerType getTriggerType() const = 0;
	virtual void pick(Actor* actor, GameTime time);
	
protected:
	bool hasPositionChanged() const;

private:
	String _label;
	bool _isActive;
	GameTime _activationTime;

	void setNextActivationTime(GameTime time);
	void activate(GameTime time);
	void deactivate(GameTime time);
};


class ArmorPack : public Trigger {
public:
	ArmorPack(const Vector2& position, const String& label);
	TriggerType getTriggerType() const;
	void pick(Actor* actor, GameTime time);
};


class AmmoPack : public Trigger {
public:
	AmmoPack(const String& weaponName, const Vector2& position, const String& label);
	TriggerType getTriggerType() const;
	String getWeaponType() const;
	void pick(Actor* actor, GameTime time);

private:
	String _weaponName;
};


class MedPack : public Trigger {
public:
	MedPack(const Vector2& position, const String& label);
	void pick(Actor* actor, GameTime time);
	TriggerType getTriggerType() const;
};
