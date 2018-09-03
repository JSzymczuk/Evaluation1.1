#pragma once

#include <map>
#include <queue>
#include "Entity.h"
#include "Weapon.h"
#include "engine/Logger.h"

class MissileManager;
class Team;
class Action;
class Trigger;
class Wall;

class Actor : public GameDynamicObject {
public:
	Actor(const std::string& name, Team* team, const Vector2& position);
	~Actor();

	String getName() const;
	Team* getTeam() const;
	float getArmor() const;
	int getRemainingArmorShots() const;
	float getHealth() const;
	String getCurrentWeapon() const;
	WeaponState& getWeaponState(const String& weaponName);
	const WeaponState& getWeaponState(const String& weaponName) const;
	//bool isWeaponLoaded(WeaponType weaponType) const;
	Vector2 getDestination() const;
	float getMaxSpeed() const;
	std::vector<Actor*> getSeenActors() const;
	void setPreferredVelocity(const Vector2& velocity);
	Vector2 getPreferredVelocity() const;
	Vector2 getVelocity() const;

	Action* getCurrentAction() const;
	bool setCurrentAction(Action* action);

	void setCurrentWeapon(const String& weaponName);
	void setAmmo(const String& weaponName, int value);
	void setArmor(float value);
	void setRemainingArmorShots(int value);

	float heal(float health);
	float damage(float dmg);
	void registerKill(Actor* actor);
	//void shootAt(const Vector2& point, GameTime time);
	void lookAt(const Vector2& point);
	void move(const std::queue<Vector2>& path);
	void stop();

	void update(GameTime time);

	bool isDead() const;
	bool isMoving() const;
	bool isRotating() const;
	bool hasPositionChanged() const;

	Aabb getAabb() const;
	float getRadius() const;
	bool isStaticElement() const;
	bool isSolid() const;
	GameDynamicObjectType getGameObjectType() const;

#ifdef _DEBUG
	std::queue<Vector2> getCurrentPath() const {
		return _path;
	}
#endif
	
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

	Vector2 _velocity;
	float _rotation;
	Vector2 _preferredVelocity;
	std::queue<Vector2> _path;
	Vector2 _lastDestination;
	Vector2 _nextSafeGoal;

	bool _isRotating;
	float _desiredOrientation;
	bool _isWaiting;
	GameTime _waitingStarted;
	int _recalculations;

	Action* _currentAction;

	float calculateRotation() const;
	void calculatePreferredVelocity();
	void clearCurrentAction();

	GameTime _lastUpdate;
	std::vector<Actor*> _seenActors;

	struct MovementCheckResult {
		bool allowed;
		std::vector<Trigger*> triggers;
	};

#ifdef _DEBUG
public:
#endif
	struct Candidate {
		Vector2 velocity;
		float difference;
		float collisionFreeDistance;
	};

#ifdef _DEBUG
private:
#endif	
	void updateMovement(GameTime time);
	void updateOrientation(GameTime time);
	void updateWeapons(GameTime time);
	void updateSpotting();
	MovementCheckResult checkMovement() const;
	float getDistanceToGoal() const;
#ifdef _DEBUG
public:
#endif
	std::pair<Vector2, Vector2> getViewBorders() const;
	std::vector<Actor*> getActorsInViewAngle() const;
	std::vector<VelocityObstacle> getVelocityObstacles(const std::vector<Actor*>& obstacles) const;
	std::vector<Candidate> computeCandidates(const std::vector<VelocityObstacle>& vo) const;
	std::vector<Wall> getWallsNearGoal() const;
	Vector2 getNextSafeGoal() const;
#ifdef _DEBUG
private:
#endif	
	float minDistanceWithoutCollision(const Vector2& direction, float maxDistance) const;
	Vector2 selectVelocity(const std::vector<Candidate>& candidates) const;

	friend class Team;
};
