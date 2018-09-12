#pragma once

#include "engine/CollisionResolver.h"

class GameDynamicObject;
class Wall;
class GameMap;

// Za³o¿enie: maksymalny promieñ obiektu dynamicznego jest mniejszy od rozmiaru regionu siatki.

class RegularGrid : public CollisionResolver {
public:
	RegularGrid();
	~RegularGrid();

	std::vector<GameDynamicObject*> initialize(GameMap* map, size_t regionSize, std::vector<GameDynamicObject*> objects);

	void add(GameStaticObject* element) override;
	void add(GameDynamicObject* element) override;
	void remove(GameDynamicObject* element) override;
	void update(GameDynamicObject* element) override;

	//std::vector<GameDynamicObject*> broadphase(const Aabb& area) const override;
	//std::vector<GameDynamicObject*> broadphase(const Vector2& point) const override;

	std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& point, float radius) const override;
	std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to) const override;
	std::vector<GameDynamicObject*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const override;

	std::vector<GameStaticObject*> broadphaseStatic(const Vector2& point, float radius) const override;
	std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to) const override;
	std::vector<GameStaticObject*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const override;

#ifdef _DEBUG
public:
#else
private:
#endif
	struct Region {
		size_t idX;
		size_t idY;
		std::vector<GameDynamicObject*> dynamicObjects;
		std::vector<GameStaticObject*> staticObjects;
	};

	const Region* getRegionById(size_t i, size_t j) const;
	const Region* getRegionByCoordinates(float x, float y) const;
	std::vector<const Region*> getRegionsContaining(const Vector2& point, float radius) const;
	std::vector<const Region*> getRegionsContaining(const Segment& segment) const;
	std::vector<const Region*> getRegionsForMovement(const Vector2& from, const Vector2& to, float radius) const;

private:
	Region** _regions;
	size_t _regionSize;
	size_t _regionsX;
	size_t _regionsY;
	float _width;
	float _height;

	Region* getRegionById(size_t i, size_t j);
	Region* getRegionByCoordinates(float x, float y);
	std::vector<Region*> getRegionsContaining(const Vector2& point, float radius);
	std::vector<Region*> getRegionsContaining(const Segment& segment);

	std::vector<Region*> getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius);
	std::vector<const Region*> getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius) const;
};
