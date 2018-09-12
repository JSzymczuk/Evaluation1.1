#pragma once

#include "math/Aabb.h"
#include "entities/Entity.h"

class CollisionResolver {
public:
	virtual void add(GameDynamicObject* element) = 0;
	virtual void remove(GameDynamicObject* element) = 0;
	virtual void update(GameDynamicObject* element) = 0;

	virtual void add(GameStaticObject* element) = 0;
	virtual void remove(GameStaticObject* element) = 0;
	virtual void update(GameStaticObject* element) = 0;

	virtual std::vector<GameDynamicObject*> broadphase(const Aabb& area) const = 0;
	virtual std::vector<GameDynamicObject*> broadphase(const Vector2& point) const = 0;

	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& point, float radius) const = 0;
	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const = 0;

	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& point, float radius) const = 0;
	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const = 0;
};