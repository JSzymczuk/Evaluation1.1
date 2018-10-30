#include "Entity.h"
#include "engine/AabbTree.h"
#include "engine/SegmentTree.h"
#include "engine/CollisionResolver.h"
#include "entities/Actor.h"
#include "entities/Trigger.h"
#include "engine/CommonFunctions.h"

void CollisionInvoker::invokeCollision(CollisionResponder* responder, GameTime time) {
	responder->onCollision(this, time);
}

void CollisionInvoker::invokeCollision(std::vector<CollisionResponder*> responders, GameTime time) {
	for (CollisionResponder* responder : responders) {
		responder->onCollision(this, time);
	}
}

StaticEntity::StaticEntity(const Aabb& aabb) : _aabb(aabb) {}

Aabb StaticEntity::getAabb() const { return _aabb; }

bool StaticEntity::isStaticElement() const { return true; }

DynamicEntity::DynamicEntity() { }

DynamicEntity::DynamicEntity(const Vector2& position, float orientation)
	: _position(position), _orientation(orientation) { }

DynamicEntity::~DynamicEntity() { }

Vector2 DynamicEntity::getPosition() const { return _position; }

float DynamicEntity::getOrientation() const { return _orientation; }

void DynamicEntity::setCollisionResolver(CollisionResolver* collisionResolver) {
	_collisionResolver = collisionResolver;
}

CollisionResolver* DynamicEntity::getCollisionResolver() const { return _collisionResolver; }

void DynamicEntity::unsetCollisionResolver() {
	if (_collisionResolver != nullptr) {
		_collisionResolver = nullptr;
	}
}

bool checkCollision(const StaticEntity* entity, const Segment& segment) {
	Vector2 v;
	for (const Segment& seg : entity->getBounds()) {
		if (common::testSegments(segment, seg, v)) {
			return true;
		}
	}
	return false;
}

float getSqDistanceTo(const StaticEntity* entity, const Segment& segment) {
	auto bounds = entity->getBounds();
	size_t n = bounds.size();
	if (n == 0) { return 0; }
	float minDist = common::sqDist(segment, bounds.at(0));
	for (size_t i = 1; i < n; ++i) {
		float dist = common::sqDist(segment, bounds.at(i));
		if (dist < minDist) {
			minDist = dist;
		}
	}
	return minDist;
}

float getDistanceTo(const StaticEntity* entity, const Vector2& point) {
	auto bounds = entity->getBounds();
	size_t n = bounds.size();
	if (n == 0) { return 0; }
	float minDist = common::distance(point, bounds.at(0));
	for (size_t i = 1; i < n; ++i) {
		float dist = common::distance(point, bounds.at(i));
		if (dist < minDist) {
			minDist = dist;
		}
	}
	return minDist;
}


bool Spotter::isSpotting() const { return true; }

std::vector<Spottable*> Spotter::getNearbyObjects() const {
	std::vector<Spottable*> result;
	Vector2 pos = getPosition();
	float sightRadius = getSightRadius();
	float maxDist = common::sqr(sightRadius);

	CollisionResolver* collisionResolver = getCollisionResolver();
	auto selfMovable = dynamic_cast<const Movable*>(this);

	auto spottables = getSpottableInArea(collisionResolver, pos, sightRadius);
	for (Spottable* entity : spottables) {
		if (entity != this && getStaticObjectsOnLine(collisionResolver, Segment(pos, entity->getPosition())).empty()) {
			result.push_back(entity);
		}
	}

	return result;
}

void Spotter::update(GameTime time) {
	_spottedObjects = getNearbyObjects();
	// Podczas ruchu zaktualizuj zbiór widzianych aktorów
	//if (hasPositionChanged()) {
		//auto actorsNearby = getNearbyObjects();

		//size_t notSpottedPreviously = _spottedObjects.size();
		//size_t notSpottedNow = actorsNearby.size();

		// Uaktualnij informacjê o s¹siedztwie w przypadku aktorów statycznych.
		//for (Spottable* spottable : _spottedObjects) {
		//	if (spottable->isSpotting()) {
		//		Spotter* spotter = dynamic_cast<Spotter*>(spottable);
		//		if (spotter == nullptr) { continue; }

		//		// Je¿eli aktor siê przemieszcza, to i tak sam wymieni ca³y wektor.
		//		if (spotter->hasPositionChanged()) { continue; }

		//		// Je¿eli siê nie porusza, to ustal czy aktor znikn¹³ czy pojawi³ siê w pobli¿u.
		//		size_t wasSpotted = common::indexOf(_spottedObjects, spottable);
		//		size_t isSpotted = common::indexOf(actorsNearby, spottable);

		//		if (wasSpotted == notSpottedPreviously && isSpotted != notSpottedNow) {
		//			spotter->spot(this);
		//		}
		//		else if (wasSpotted != notSpottedPreviously && isSpotted == notSpottedNow) {
		//			spotter->unspot(this);
		//		}
		//	}
		//}

		//_spottedObjects = actorsNearby;
	//}
}

std::vector<Spottable*> Spotter::getSpottedObjects() const { return _spottedObjects; }

void Spotter::spot(Spottable* entity) {
	_spottedObjects.push_back(entity);
}

void Spotter::unspot(Spottable* entity) {
	size_t n = _spottedObjects.size();
	size_t k = common::indexOf(_spottedObjects, entity);
	if (k != n) {
		if (k != n - 1) {
			_spottedObjects[k] = _spottedObjects.at(n - 1);
			_spottedObjects.pop_back();
		}
	}
}

Aabb DynamicEntity::getAabb() const {
	Vector2 p = getPosition();
	float r = Config.ActorRadius;
	return Aabb(p.x - r, p.y - r, 2 * r, 2 * r);
}

bool DynamicEntity::isStaticElement() const { return true; }