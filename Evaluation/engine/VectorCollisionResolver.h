#pragma once

#include "engine/CollisionResolver.h"

class VectorCollisionResolver : public CollisionResolver {
public:
	VectorCollisionResolver();
	~VectorCollisionResolver();

	void initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) override;

	void add(DynamicEntity* element) override;
	void add(StaticEntity* element) override;
	void remove(DynamicEntity* element) override;
	void update(Movable* element) override;

	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& point, float radius) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const override;

	std::vector<StaticEntity*> broadphaseStatic(const Vector2& point, float radius) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const override;

private:
	std::vector<DynamicEntity*> _dynamic;
	std::vector<StaticEntity*> _static;
};
