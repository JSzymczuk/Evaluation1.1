#pragma once

#include "math/Aabb.h"
#include "entities/Entity.h"

typedef std::pair<std::vector<GameDynamicObject*>, std::vector<GameDynamicObject*>> EntitiesInitializeResult;

class CollisionResolver {
public:
	//virtual EntitiesInitializeResult initialize(const std::vector<GameDynamicObject*>& objects) = 0;
	virtual bool isPositionValid(const GameDynamicObject* entity) const = 0;

	virtual void add(GameDynamicObject* element) = 0;
	virtual void remove(GameDynamicObject* element) = 0;
	virtual void update(GameDynamicObject* element) = 0;
	virtual void add(GameStaticObject* element) = 0;

	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& point) const = 0;
	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& point, float radius) const = 0;
	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const = 0;

	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& point) const = 0;
	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& point, float radius) const = 0;
	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to) const = 0;
	virtual std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const = 0;
};