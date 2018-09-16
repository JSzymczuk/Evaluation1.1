#pragma once

#include "math/Math.h"
#include "main/Configuration.h"
#include <vector>

enum GameDynamicObjectType {
	ACTOR,
	TRIGGER
};

class CollisionResolver;

class GameObject {
public:
	virtual bool isStaticElement() const = 0;
};

class GameStaticObject : public GameObject {
public:
	virtual std::vector<Segment> getBounds() const = 0;
	virtual bool checkCollision(const Segment& segment) const = 0;
	virtual float getDistanceTo(const Vector2& point) const = 0;
	virtual float getSqDistanceTo(const Segment& segment) const = 0;
	bool isStaticElement() const override;
};

#include <map>

class GameDynamicObject : public GameObject {
public:

	static std::map<const GameDynamicObject*, String> actorLogs;

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
	bool isStaticElement() const override;

	virtual void update(GameTime gameTime);

	virtual Aabb getAabb() const = 0;
	virtual float getRadius() const = 0;
	virtual bool isSolid() const = 0;
	virtual GameDynamicObjectType getGameObjectType() const = 0;

	virtual bool isSpotting() const;
	virtual void spot(GameDynamicObject* entity);
	virtual void unspot(GameDynamicObject* entity);

protected:
	Vector2 _position;
	Vector2 _velocity;
	float _orientation;
	float _rotation;

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