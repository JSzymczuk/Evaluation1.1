#pragma once

#include "math/Aabb.h"
#include "entities/Entity.h"

class Actor;
class Movable;
class Trigger;
class Spottable;
class Destructible;

class CollisionResolver {
public:
	virtual ~CollisionResolver();

	virtual void initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) = 0;

	virtual void add(StaticEntity* element) = 0;
	virtual void add(DynamicEntity* trigger) = 0;
	virtual void remove(DynamicEntity* actor) = 0;
	virtual void update(Movable* actor) = 0;

	virtual std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& point, float radius) const = 0;
	virtual std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const = 0;

	virtual std::vector<StaticEntity*> broadphaseStatic(const Vector2& point, float radius) const = 0;
	virtual std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const = 0;
	
protected:
	void addDynamicObject(DynamicEntity* entity);
	void removeDynamicObject(DynamicEntity* entity);

};

std::vector<DynamicEntity*> getDynamicObjectsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);
std::vector<StaticEntity*> getStaticObjectsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);
std::vector<DynamicEntity*> getDynamicObjectsOnLine(const CollisionResolver* collisionResolver, const Segment& segment);
std::vector<StaticEntity*> getStaticObjectsOnLine(const CollisionResolver* collisionResolver, const Segment& segment);
std::vector<Trigger*> getTriggersInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);
std::vector<Actor*> getActorsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);
std::vector<Spottable*> getSpottableInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);
std::vector<Destructible*> getDestructibleInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius);

std::vector<DynamicEntity*> narrowphaseDynamic(const CollisionResolver* collisionResolver, const DynamicEntity* entity);
std::vector<StaticEntity*> narrowphaseStatic(const CollisionResolver* collisionResolver, const DynamicEntity* entity);
bool isPositionValid(const CollisionResolver* collisionResolver, const DynamicEntity* entity, bool staticOnly = false);
bool checkMovementCollisions(const CollisionResolver* collisionResolver, const Segment& segment, float margin);