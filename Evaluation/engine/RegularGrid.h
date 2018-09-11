#pragma once

#include <vector>
#include "math/Math.h"

class GameDynamicObject;
class Wall;
class GameMap;

// Za³o¿enie: maksymalny promieñ obiektu dynamicznego jest mniejszy od rozmiaru regionu siatki.

class RegularGrid {
public:
	RegularGrid();
	~RegularGrid();

	std::vector<GameDynamicObject*> initialize(GameMap* map, size_t regionSize, std::vector<GameDynamicObject*> objects);
	void add(GameDynamicObject* entity);
	void remove(GameDynamicObject* entity);
	void update(GameDynamicObject* entity, const Vector2& previousPosition);
	std::vector<GameDynamicObject*> broadphase(const Vector2& point, float radius);

#ifdef _DEBUG
public:
#else
private:
#endif
	struct Region {
		size_t idX;
		size_t idY;
		std::vector<GameDynamicObject*> dynamicObjects;
		std::vector<Wall*> staticObjects;
	};

	Region* getRegionById(size_t i, size_t j);
	Region* getRegionByCoordinates(float x, float y);
	std::vector<Region*> getRegionsContaining(const Vector2& point, float radius);
	std::vector<Region*> getRegionsContaining(const Segment& segment);
	std::vector<Region*> getRegionsForMovement(const Vector2& from, const Vector2& to, float radius);

private:
	Region** _regions;
	size_t _regionSize;
	size_t _regionsX;
	size_t _regionsY;
	float _width;
	float _height;

	std::vector<Region*> getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius);
};
