#include "VectorCollisionResolver.h"

VectorCollisionResolver::VectorCollisionResolver() {}

VectorCollisionResolver::~VectorCollisionResolver() {}

// Operacje na elementach

void VectorCollisionResolver::initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) {
	_dynamic = dynamicObjects;
	for (DynamicEntity* entity : dynamicObjects) {
		CollisionResolver::addDynamicObject(entity);
	}
}

void VectorCollisionResolver::add(StaticEntity* element) {
	_static.push_back(element);
}

void VectorCollisionResolver::add(DynamicEntity* element) {
	_dynamic.push_back(element);
	CollisionResolver::addDynamicObject(element);
}

void VectorCollisionResolver::remove(DynamicEntity* element) {
}

void VectorCollisionResolver::update(Movable* element) {
}

// Faza ogólna statyczna

std::vector<DynamicEntity*> VectorCollisionResolver::broadphaseDynamic(const Vector2& point, float radius) const {
	return _dynamic; 
}
std::vector<DynamicEntity*> VectorCollisionResolver::broadphaseDynamic(const Vector2& from, const Vector2& to) const {
	return _dynamic; 
}
std::vector<DynamicEntity*> VectorCollisionResolver::broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const {
	return _dynamic;
}

// Faza ogólna statyczna

std::vector<StaticEntity*> VectorCollisionResolver::broadphaseStatic(const Vector2& point, float radius) const {
	return _static; 
}
std::vector<StaticEntity*> VectorCollisionResolver::broadphaseStatic(const Vector2& from, const Vector2& to) const {
	return _static;
}

std::vector<StaticEntity*> VectorCollisionResolver::broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const {
	return _static;
}
