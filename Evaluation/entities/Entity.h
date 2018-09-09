#pragma once

#include "math/Math.h"
#include "main/Configuration.h"
#include "Wall.h"
#include <vector>

enum GameDynamicObjectType {
	ACTOR,
	TRIGGER
};

template <typename T> class AabbTree;
template <typename T> class SegmentTree;

class GameDynamicObject {
public:
	GameDynamicObject();
	GameDynamicObject(const Vector2& position, float orientation);
	virtual ~GameDynamicObject();

	Vector2 getPosition() const;
	float getOrientation() const;
	common::Circle getCollisionArea() const;
	void enableCollisions(AabbTree<GameDynamicObject*>* collisionTree, SegmentTree<Wall>* segmentTree);
	void disableCollisions();

	virtual void update(GameTime gameTime);

	virtual Aabb getAabb() const = 0;
	virtual float getRadius() const = 0;
	virtual bool isSolid() const = 0;
	virtual bool isStaticElement() const = 0;
	virtual GameDynamicObjectType getGameObjectType() const = 0;

protected:
	Vector2 _position;
	float _orientation;

	virtual bool hasPositionChanged() const = 0;
	std::vector<GameDynamicObject*> checkCollisions(const Aabb& aabb) const;
	std::vector<Wall> checkCollisions(const Segment& segment) const;

	// wyrzuciæ to st¹d
	bool checkMovementCollisions(const Segment& segment, float minDistance) const;

private:
	AabbTree<GameDynamicObject*>* _collisionTree = nullptr;
	SegmentTree<Wall>* _segmentTree = nullptr;
};

struct VelocityObstacle {
	Vector2 apex;
	Vector2 side1;
	Vector2 side2;
	GameDynamicObject* obstacle;
};