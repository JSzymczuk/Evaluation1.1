#pragma once

#include "math/Math.h"
#include "main/Configuration.h"
#include "Wall.h"
#include <vector>

enum GameDynamicObjectType {
	ACTOR,
	TRIGGER
};

class CollisionResolver;

class GameObject { };

class GameStaticObject : public GameObject { 
public:
	virtual std::vector<Segment> getBounds() const = 0;
	virtual bool checkCollision(const Segment& segment) const = 0;
	virtual float getDistanceTo(const Vector2& point) const = 0;
	virtual float getSqDistanceTo(const Segment& segment) const = 0;
};

class GameDynamicObject : public GameObject {
public:
	GameDynamicObject();
	GameDynamicObject(const Vector2& position, float orientation);
	virtual ~GameDynamicObject();

	Vector2 getPosition() const;
	Vector2 getVelocity() const;
	float getOrientation() const;
	common::Circle getCollisionArea() const;
	bool hasPositionChanged() const;
	void enableCollisions(CollisionResolver* collisionResolver);
	void disableCollisions();

	virtual void update(GameTime gameTime);

	virtual Aabb getAabb() const = 0;
	virtual float getRadius() const = 0;
	virtual bool isSolid() const = 0;
	virtual bool isStaticElement() const = 0;
	virtual GameDynamicObjectType getGameObjectType() const = 0;

protected:
	Vector2 _position;
	Vector2 _velocity;
	float _orientation;

	bool checkMovementCollisions(const Segment& segment, float margin) const;
	std::vector<GameDynamicObject*> getDynamicObjectsInArea(const Vector2& point, float radius) const;
	std::vector<GameStaticObject*> getStaticObjectsInArea(const Vector2& point, float radius) const;
	std::vector<GameDynamicObject*> getDynamicObjectsOnLine(const Segment& segment) const;
	std::vector<GameStaticObject*> getStaticObjectsOnLine(const Segment& segment) const;
	
private:
	CollisionResolver* _collisionResolver = nullptr;
};

struct VelocityObstacle {
	Vector2 apex;
	Vector2 side1;
	Vector2 side2;
	GameDynamicObject* obstacle;
};