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
class Game;

class Actor : public GameDynamicObject {
public:
	Actor(const std::string& name, const Vector2& position);
	~Actor();

	String getName() const;
	Team* getTeam() const;
	float getArmor() const;
	int getRemainingArmorShots() const;
	float getHealth() const;
	String getCurrentWeapon() const;
	WeaponState& getWeaponState(const String& weaponName);
	const WeaponState& getWeaponState(const String& weaponName) const;
	Vector2 getDestination() const;
	float getMaxSpeed() const;
	std::vector<GameDynamicObject*> getSeenObjects() const;
	Vector2 getPreferredVelocity() const;
	Vector2 getVelocity() const;
	Vector2 getShortGoal() const;
	Vector2 getLongGoal() const; 
	float estimateRemainingDistance() const;

	Action* getCurrentAction() const;
	bool setCurrentAction(Action* action);
	void setNextAction(Action* action);

	void setAmmo(const String& weaponName, int value);
	void setArmor(float value);
	void setRemainingArmorShots(int value);

	float heal(float health);
	float damage(float dmg);
	void registerKill(Actor* actor);

	void update(GameTime time);

	bool isDead() const;
	bool isMoving() const;
	bool isRotating() const;
	bool hasPositionChanged() const;

	Aabb getAabb() const override;
	float getRadius() const override;
	bool isSolid() const override;
	GameDynamicObjectType getGameObjectType() const override;

	bool isSpotting() const override;
	void spot(GameDynamicObject* object) override;
	void unspot(GameDynamicObject* object) override;

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

	Vector2 _preferredVelocity;
	std::queue<Vector2> _path;
	Vector2 _lastDestination;
	Vector2 _nextSafeGoal;

	std::vector<GameDynamicObject*> _nearbyObjects;

	bool _isRotating;
	float _desiredOrientation;
	bool _isWaiting;
	GameTime _waitingStarted;
	int _recalculations;
	std::vector<Vector2> _positionHistory;
	size_t _nextHistoryIdx;
	size_t _positionHistoryLength;
	Action* _currentAction;
	Action* _nextAction;

	void lookAt(const Vector2& point);
	void move(const std::queue<Vector2>& path);
	void stop();
	void setPreferredVelocity(const Vector2& velocity);
	void setCurrentWeapon(const String& weaponName);

	float calculateRotation() const;
	void setPreferredVelocityAndSafeGoal();
	void clearCurrentAction();	

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

	std::queue<Vector2> getCurrentPath() const { return _path; }
	std::pair<Vector2, Vector2> getViewBorders() const;
	std::vector<GameDynamicObject*> getObjectsInViewAngle() const;
	std::vector<VelocityObstacle> getVelocityObstacles(const std::vector<GameDynamicObject*>& obstacles) const;
	std::vector<Candidate> computeCandidates(const std::vector<VelocityObstacle>& vo) const;
	std::vector<Wall*> getWallsNearGoal() const;
	Vector2 getNextSafeGoal() const;

#ifdef _DEBUG
private:
#endif	
	void updateMovement(GameTime time);
	bool updateOrientation(GameTime time);
	bool updateWeapons(GameTime time);
	bool updateCurrentAction(GameTime time);
	void updateSpotting();
	MovementCheckResult checkMovement() const;
	float getDistanceToGoal() const;
	std::vector<GameDynamicObject*> getNearbyObjects() const;
	void abortMovement(String loggerMessage, bool resetCounter);
	void saveCurrentPositionInHistory();
	void clearPositionHistory();
	bool isOscilating() const;
	float minDistanceWithoutCollision(const Vector2& direction, float maxDistance) const;
	Vector2 selectVelocity(const std::vector<Candidate>& candidates) const;

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
