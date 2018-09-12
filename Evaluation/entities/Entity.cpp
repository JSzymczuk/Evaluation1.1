#include "Entity.h"
#include "engine/AabbTree.h"
#include "engine/SegmentTree.h"

GameDynamicObject::GameDynamicObject() { }

GameDynamicObject::GameDynamicObject(const Vector2& position, float orientation)
	: _position(position), _orientation(orientation) { }

GameDynamicObject::~GameDynamicObject() { }

Vector2 GameDynamicObject::getPosition() const { return _position; }

float GameDynamicObject::getOrientation() const { return _orientation; }

Vector2 GameDynamicObject::getVelocity() const { return _velocity; }

bool GameDynamicObject::hasPositionChanged() const { return _velocity.lengthSquared() <= common::EPSILON; }

common::Circle GameDynamicObject::getCollisionArea() const { return { _position, getRadius() }; }

std::vector<GameDynamicObject*> GameDynamicObject::getDynamicObjectsInArea(const Vector2& point, float radius) const {
	std::vector<GameDynamicObject*> result;
	if (_collisionResolver != nullptr) { 
		for (GameDynamicObject* elem : _collisionResolver->broadphase(point, radius)) {
			if (common::sqDist(point, elem->getPosition()) <= common::sqr(radius + elem->getRadius())) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

std::vector<GameStaticObject*> GameDynamicObject::getStaticObjectsInArea(const Vector2& point, float radius) const {
	std::vector<GameStaticObject*> result;
	if (_collisionResolver != nullptr) {
		for (GameStaticObject* elem : _collisionResolver->broadphaseStatic(point, radius)) {
			if (elem->getDistanceTo(point) <= radius + common::EPSILON) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

std::vector<GameDynamicObject*> GameDynamicObject::getDynamicObjectsOnLine(const Segment& segment) const {
	std::vector<GameDynamicObject*> result;
	if (_collisionResolver != nullptr) {
		for (GameDynamicObject* elem : _collisionResolver->broadphase(segment.from, segment.to)) {
			if (common::distance(elem->getPosition(), segment) <= elem->getRadius() + common::EPSILON) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

std::vector<GameStaticObject*> GameDynamicObject::getStaticObjectsOnLine(const Segment& segment) const {
	std::vector<GameStaticObject*> result;
	if (_collisionResolver != nullptr) {
		for (GameStaticObject* elem : _collisionResolver->broadphaseStatic(segment.from, segment.to)) {
			if (elem->checkCollision(segment)) {
				result.push_back(elem);
			}
		}
	}
	return result;
}

bool GameDynamicObject::checkMovementCollisions(const Segment& segment, float margin) const {
	float r2 = margin * margin;
	if (_collisionResolver != nullptr) { 
		for (GameStaticObject* entity : _collisionResolver->broadphaseStatic(segment.from, segment.to, margin)) {
			if (entity->getSqDistanceTo(segment) <= r2) {
				//Logger::log(std::to_string(sqrtf(common::sqDist(segment, wall.getSegment()))));
				return true;
			}
		}
	}
	return false;
}

void GameDynamicObject::enableCollisions(CollisionResolver* collisionResolver) {
	_collisionResolver = collisionResolver;
}

void GameDynamicObject::disableCollisions() {
	if (_collisionResolver != nullptr) {
		_collisionResolver->remove(this);
		_collisionResolver = nullptr;
	}
}

void GameDynamicObject::update(GameTime gameTime) {
	if (!isStaticElement() && _collisionResolver != nullptr && hasPositionChanged()) {
		_collisionResolver->update(this);
	}
}