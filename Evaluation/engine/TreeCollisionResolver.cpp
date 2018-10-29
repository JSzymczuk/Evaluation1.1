#include "TreeCollisionResolver.h"
#include "entities/Actor.h"
#include "entities/Trigger.h"

TreeCollisionResolver::TreeCollisionResolver() {}

TreeCollisionResolver::~TreeCollisionResolver() {}

void TreeCollisionResolver::initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) {
	AabbTree<DynamicEntity*>::initialize(&_dynamic, dynamicObjects, Config.AabbTreeMargin);
	for (DynamicEntity* entity : dynamicObjects) {
		CollisionResolver::addDynamicObject(entity);
	}
	for (StaticEntity* entity : _staticToAdd) {
		_static.insert(entity);
	}
}

void TreeCollisionResolver::add(DynamicEntity* element) {
	_dynamic.insert(element);
	CollisionResolver::addDynamicObject(element);
}

void TreeCollisionResolver::add(StaticEntity* element) {
	_staticToAdd.push_back(element);
}

void TreeCollisionResolver::remove(DynamicEntity* element) {
	_dynamic.remove(element);
	CollisionResolver::removeDynamicObject(element);
}

void TreeCollisionResolver::update(Movable* element) {
	if (_dynamic.update(element)) { }
}

std::vector<DynamicEntity*> TreeCollisionResolver::broadphaseDynamic(const Vector2& point, float radius) const {
	return _dynamic.broadphase(Aabb(point.x - radius, point.y - radius, 2 * radius, 2 * radius));
}
std::vector<DynamicEntity*> TreeCollisionResolver::broadphaseDynamic(const Vector2& from, const Vector2 & to) const {
	return _dynamic.broadphase(Aabb(from, to));
}
std::vector<DynamicEntity*> TreeCollisionResolver::broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const {
	return _dynamic.broadphase(Aabb(from, to).inflate(radius));
}

std::vector<StaticEntity*> TreeCollisionResolver::broadphaseStatic(const Vector2 & point, float radius) const {
	return _static.broadphase(Aabb(point.x - radius, point.y - radius, 2 * radius, 2 * radius));
}

std::vector<StaticEntity*> TreeCollisionResolver::broadphaseStatic(const Vector2 & from, const Vector2 & to) const {
	return _static.broadphase(Aabb(from, to));
}

std::vector<StaticEntity*> TreeCollisionResolver::broadphaseStatic(const Vector2 & from, const Vector2 & to, float radius) const {
	return _static.broadphase(Aabb(from, to).inflate(radius));
}


std::vector<Aabb> TreeCollisionResolver::getDynamicAabbs() const { return _dynamic.getAabbs(); }

std::vector<Aabb> TreeCollisionResolver::getStaticAabbs() const { return _static.getAabbs(); }