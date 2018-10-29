#pragma once

#include "Entity.h"

enum TriggerType {
	HEALTH,
	WEAPON,
	ARMOR,

	TRIGGERS_SIZE
};

class Actor;

class Trigger : public DynamicEntity, 
	public virtual Updatable, 
	public virtual CollisionResponder {
public:
	Trigger(const Vector2& position, const String& label);
	~Trigger();

	virtual String getName() const = 0;
	bool isSolid() const override;
	bool isSpotting() const override;
	float getRadius() const override;
	
	bool isActive() const;
	void update(GameTime time) override;
	void onCollision(CollisionInvoker* invoker, GameTime time) override;

	virtual TriggerType getTriggerType() const = 0;
	
protected:
	virtual void pick(Actor* actor, GameTime time) = 0;

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
	String getName() const;
	void pick(Actor* actor, GameTime time);
	TriggerType getTriggerType() const override;
};


class AmmoPack : public Trigger {
public:
	AmmoPack(const String& weaponName, const Vector2& position, const String& label);
	String getName() const;
	String getWeaponType() const;
	void pick(Actor* actor, GameTime time);
	TriggerType getTriggerType() const override;

private:
	String _weaponName;
};


class MedPack : public Trigger {
public:
	MedPack(const Vector2& position, const String& label);
	void pick(Actor* actor, GameTime time);
	String getName() const;
	TriggerType getTriggerType() const override;
};
