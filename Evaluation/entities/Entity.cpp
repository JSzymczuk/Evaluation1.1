#include "Entity.h"
#include "engine/AabbTree.h"
#include "engine/SegmentTree.h"

GameDynamicObject::GameDynamicObject() { }

GameDynamicObject::GameDynamicObject(const Vector2& position, float orientation)
	: _position(position), _orientation(orientation) { }

GameDynamicObject::~GameDynamicObject() { }

common::Circle GameDynamicObject::getCollisionArea() const { return { _position, getRadius() }; }

std::vector<GameDynamicObject*> GameDynamicObject::checkCollisions(const Aabb& aabb) const {
	if (_collisionTree != nullptr) { return _collisionTree->broadphase(aabb); }
	return std::vector<GameDynamicObject*>();
}

std::vector<Wall> GameDynamicObject::checkCollisions(const Segment& segment) const {
	if (_segmentTree != nullptr) { return narrowphase(segment, _segmentTree->broadphase(segment)); }
	return std::vector<Wall>();
}

#include "engine/Logger.h"

bool GameDynamicObject::checkMovementCollisions(const Segment& segment, float minDistance) const {
	for (Wall& wall : _segmentTree->getElements()) {
		if (common::sqDist(segment, wall.getSegment()) <= minDistance * minDistance) {
			Logger::log(std::to_string(sqrtf(common::sqDist(segment, wall.getSegment()))));
			return true;
		}
	}
	return false;
}

void GameDynamicObject::update(GameTime gameTime) {
	if (!isStaticElement() && _collisionTree != nullptr && hasPositionChanged()) {
		_collisionTree->update(this);
	}
}

void GameDynamicObject::enableCollisions(AabbTree<GameDynamicObject*>* collisionTree, SegmentTree<Wall>* segmentTree) {
	_collisionTree = collisionTree;
	_segmentTree = segmentTree;
}

void GameDynamicObject::disableCollisions() {
	if (_collisionTree != nullptr) {
		_collisionTree->remove(this);
		_collisionTree = nullptr;
	}
	_segmentTree = nullptr;
}