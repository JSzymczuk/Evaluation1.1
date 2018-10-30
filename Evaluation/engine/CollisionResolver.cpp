#include "engine/CollisionResolver.h"
#include "entities/Actor.h"
#include "entities/Entity.h"
#include "entities/Trigger.h"

CollisionResolver::~CollisionResolver() {}

void CollisionResolver::addDynamicObject(DynamicEntity* entity) { entity->setCollisionResolver(this); }

void CollisionResolver::removeDynamicObject(DynamicEntity* entity) { entity->unsetCollisionResolver(); }

std::vector<DynamicEntity*> getDynamicObjectsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	std::vector<DynamicEntity*> result;
	if (collisionResolver != nullptr) {

		auto broadphaseResult = collisionResolver->broadphaseDynamic(point, radius);
		
		for (DynamicEntity* elem : broadphaseResult) {
			if (common::sqDist(point, elem->getPosition()) <= common::sqr(radius + elem->getRadius())) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

template <typename T>
std::vector<T*> getObjectsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	std::vector<T*> result;
	if (collisionResolver != nullptr) {

		auto broadphaseResult = collisionResolver->broadphaseDynamic(point, radius);
		
		for (DynamicEntity* e : broadphaseResult) {
			T* t = dynamic_cast<T*>(e);
			if (t != nullptr) {
				if (common::sqDist(point, t->getPosition()) <= common::sqr(radius + t->getRadius())) {
					result.push_back(t);
				}
			}
		}
	}
	return result;
}

std::vector<Trigger*> getTriggersInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	return getObjectsInArea<Trigger>(collisionResolver, point, radius);
}

std::vector<Actor*> getActorsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	return getObjectsInArea<Actor>(collisionResolver, point, radius);
}

std::vector<Spottable*> getSpottableInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	return getObjectsInArea<Spottable>(collisionResolver, point, radius);
}

std::vector<Destructible*> getDestructibleInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	std::vector<Destructible*> result;
	float r2 = radius * radius;

	auto broadphaseResult = collisionResolver->broadphaseDynamic(point, radius);

	for (DynamicEntity* e : broadphaseResult) {
		Destructible* d = dynamic_cast<Destructible*>(e);
		if (d != nullptr && d->getSquareDistanceTo(point) <= r2) {
			result.push_back(d);
		}
	}	

	return result;
}

std::vector<StaticEntity*> getStaticObjectsInArea(const CollisionResolver* collisionResolver, const Vector2& point, float radius) {
	std::vector<StaticEntity*> result;
	if (collisionResolver != nullptr) {

		auto broadphaseResult = collisionResolver->broadphaseStatic(point, radius);
		
		for (StaticEntity* elem : broadphaseResult) {
			if (getDistanceTo(elem, point) <= radius + common::EPSILON) {
				result.push_back(elem);
			}
		}
	}

	return result;
}

std::vector<DynamicEntity*> getDynamicObjectsOnLine(const CollisionResolver* collisionResolver, const Segment& segment) {
	std::vector<DynamicEntity*> result;
	if (collisionResolver != nullptr) {

		auto broadphaseResult = collisionResolver->broadphaseDynamic(segment.from, segment.to);

		for (DynamicEntity* elem : broadphaseResult) {
			if (common::distance(elem->getPosition(), segment) <= elem->getRadius() + common::EPSILON) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

std::vector<StaticEntity*> getStaticObjectsOnLine(const CollisionResolver* collisionResolver, const Segment& segment) {
	std::vector<StaticEntity*> result;
	if (collisionResolver != nullptr) {
		
		auto broadphaseResult = collisionResolver->broadphaseStatic(segment.from, segment.to);
		
		for (StaticEntity* elem : broadphaseResult) {
			if (checkCollision(elem, segment)) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

std::vector<DynamicEntity*> narrowphaseDynamic(const CollisionResolver* collisionResolver, const DynamicEntity* entity) {
	std::vector<DynamicEntity*> result;
	Vector2 position = entity->getPosition();
	float radius = entity->getRadius() + Config.MovementSafetyMargin, r2 = radius * radius;

	auto broadphaseResult = collisionResolver->broadphaseDynamic(position, radius);

	for (DynamicEntity* other : broadphaseResult) {
		if (entity != other && common::sqDist(position, other->getPosition()) <= r2) {
			result.push_back(other);
		}
	}

	return result;
}

std::vector<StaticEntity*> narrowphaseStatic(const CollisionResolver* collisionResolver, const DynamicEntity* entity) {
	std::vector<StaticEntity*> result;
	Vector2 position = entity->getPosition();
	float radius = entity->getRadius() + Config.MovementSafetyMargin;
	
	auto broadphaseResult = collisionResolver->broadphaseStatic(position, radius);

	for (StaticEntity* other : broadphaseResult) {
		if (getDistanceTo(other, position) <= radius) {
			result.push_back(other);
		}
	}

	return result;
}

bool isPositionValid(const CollisionResolver* collisionResolver, const DynamicEntity* entity, bool staticOnly) {
	return (staticOnly || narrowphaseDynamic(collisionResolver, entity).size() == 0)
		&& narrowphaseStatic(collisionResolver, entity).size() == 0;
}

bool checkMovementCollisions(const CollisionResolver* collisionResolver, const Movable* movable, const Segment& segment) {

	float margin = movable->getRadius() + Config.MovementSafetyMargin + common::EPSILON;

	float r2 = margin * margin;
	
	auto broadphaseStaticResult = collisionResolver->broadphaseStatic(segment.from, segment.to, margin);
	for (StaticEntity* entity : broadphaseStaticResult) {
		if (getSqDistanceTo(entity, segment) <= r2) {
			return true;
		}
	}

	auto broadphaseDynamicResult = collisionResolver->broadphaseDynamic(segment.from, segment.to, margin);
	for (DynamicEntity* entity : broadphaseDynamicResult) {
		if (entity != movable && entity->isSolid() && common::distance(entity->getPosition(), segment) < margin) {
			return true;
		}
	}

	return false;
}

