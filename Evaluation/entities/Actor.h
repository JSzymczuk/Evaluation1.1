#pragma once

#include "entities/Movable.h"
#include "entities/Weapon.h"
#include "entities/Missile.h"
#include "entities/Trigger.h"
#include "entities/Team.h"
#include "engine/Logger.h"
#include "actions/Action.h"

class MissileManager;
class Team;
class Action;
class Trigger;
class Wall;
class Game;

class Actor : public Movable, 
	public virtual Destructible, 
	public virtual MissileOwner {
public:
	Actor(const String& name, const Vector2& position);
	~Actor();
	
	// W³aœciwoœci dotycz¹ce wytrzyma³oœci
	bool isDead() const;
	float getHealth() const;
	float heal(float health);

	// W³aœciwoœci dotycz¹ce pancerza
	float getArmor() const;
	void setArmor(float value);
	int getRemainingArmorShots() const;
	void setRemainingArmorShots(int value);

	// W³aœciwoœci dotycz¹ce broni
	void setAmmo(const String& weaponName, int value);
	WeaponState& getWeaponState(const String& weaponName);
	const WeaponState& getWeaponState(const String& weaponName) const;
	
	// W³aœciwoœci dotycz¹ce akcji
	Action* getCurrentAction() const;
	ActionType getCurrentActionType() const;
	bool setCurrentAction(Action* action);
	void setNextAction(Action* action);
	
	// W³aœciwoœci dotycz¹ce zaobserwowanych obiektów
	std::vector<Actor*> getSeenActors() const;
	std::vector<Trigger*> getSeenTriggers() const;
	
	// Metody wirtualne klasy Movable
	Vector2 getPosition() const override;
	float getRadius() const override;
	float getSightRadius() const override;
	bool isSolid() const override;
	bool isLookingStraight() const override;
	float getMaxSpeed() const override;
	void update(GameTime time) override;

	// Interfejs MissileOwner
	String getCurrentWeapon() const override;
	void registerKill(const Destructible* destructible) override;
	
	// Interfejs Destructible
	String getName() const override;
	Team* getTeam() const override;
	float recieveDamage(float damage) override;
	bool isDestroyed() const override;
	bool wasDestroyed() const override;
	void onDestroy() override;
	float getSquareDistanceTo(const Vector2& point) const override;

	void onCollision(CollisionInvoker* invoker, GameTime time) override;

private:
	Team* _team;
	std::string _name;
	std::string _sprite;

	float _health;
	float _armor;
	int _armorShotsRemaining;
	String _currentWeapon;
	std::map<String, WeaponState> _weapons;

	int _kills;
	int _friendkills;
	bool _isDestroyed = false;

	Action* _currentAction;
	Action* _nextAction;

	void setCurrentWeapon(const String& weaponName);
	void clearCurrentAction();	

	bool updateWeapons(GameTime time);
	bool updateCurrentAction(GameTime time);
	
	friend class Team;
	friend class MoveAction;
	friend class MoveAtAction;
	friend class WanderAction;
	friend class FaceAction;
	friend class ChangeWeaponAction;
	friend class ShootAction;
	friend class IdleAction;
	friend class DeadAction;
};
