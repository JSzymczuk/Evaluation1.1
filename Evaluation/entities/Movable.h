#pragma once

#include <map>
#include <queue>
#include "entities/Entity.h"
#include "engine/RegularGrid.h"

class CollisionResolver;
class Wall;
class CollisionInvoker;
class Spotter;
class DynamicEntity;

struct VelocityObstacle {
	Vector2 apex;
	Vector2 side1;
	Vector2 side2;
	Spottable* obstacle;
};

struct Candidate {
	Vector2 velocity;
	float difference;
	float collisionFreeDistance;
};

struct MovementCheckResult {
	bool allowed;
	std::vector<CollisionResponder*> responders;
};


class Movable : public DynamicEntity,
	public virtual CollisionInvoker, 
	public virtual CollisionResponder,
	public virtual Spotter {

public:	
	Movable(const Vector2& position);
	virtual ~Movable();

	bool isMoving()   const;
	bool isRotating() const;
	bool isSpotting() const override;

	void lookAt(const Vector2& point);
	void move(const std::queue<Vector2>& path);
	void stop();

	virtual float getMaxSpeed() const = 0;

	Vector2 getPosition()  const override;
	Vector2 getVelocity()  const;
	float   getRotation()  const;
	Vector2 getShortGoal() const;
	Vector2 getLongGoal()  const;
	Vector2 getPreferredVelocity() const;
	void    setPreferredVelocity(const Vector2& velocity);
	float   estimateRemainingDistance() const;

	void update(GameTime time) override;
	bool hasPositionChanged() const override;
	bool isStaticElement() const override;

protected:
	virtual bool isLookingStraight() const = 0;

	bool isOscilating() const;
	CollisionResolver* getCollisionResolver() const override;
	std::queue<Vector2> getCurrentPath() const;

private:
	void updateMovement(GameTime time);
	bool updateOrientation(GameTime time);
public:
	float calculateRotation() const;
	Vector2 selectVelocity(const std::vector<Candidate>& candidates) const;
	std::vector<Spottable*> getObjectsInViewAngle() const;
	MovementCheckResult checkMovement() const;
	std::vector<Candidate> computeCandidates(const std::vector<VelocityObstacle>& vo) const;
	std::vector<VelocityObstacle> getVelocityObstacles(const std::vector<Spottable*>& obstacles) const;

	float minDistanceWithoutCollision(const Vector2& direction, float maxDistance) const;
	std::pair<Vector2, Vector2> getVOSides(const Vector2& point, const common::Circle& circle) const;
	std::vector<Segment> getSegmentsNearGoal() const;
	Vector2 getNextSafeGoal() const;
	float getDistanceToGoal() const;
	void abortMovement(bool resetCounter);
	void setPreferredVelocityAndSafeGoal();

	void saveCurrentPositionInHistory();
	void clearPositionHistory();

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
	std::vector<Vector2> _positionHistory;
	size_t _nextHistoryIdx;
	size_t _positionHistoryLength;

	virtual void onCollision(CollisionInvoker* actor, GameTime time) override;

private:
	RegularGrid::Region* _gridRegion = nullptr;

	friend class RegularGrid;
};