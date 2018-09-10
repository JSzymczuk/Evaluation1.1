#pragma once

#include <vector>

class GameDynamicObject;
class Wall;
class Vector2;
class Circle;
class Segment;
class GameMap;

class RegularGrid {

	struct Region {
		size_t idX;
		size_t idY;
		std::vector<GameDynamicObject*> dynamicObjects;
		std::vector<Wall*> staticObjects;
	};

	// Za³o¿enie: maksymalny promieñ obiektu dynamicznego jest mniejszy od rozmiaru regionu.
	Region** _regions;
	size_t _regionSize;
	size_t _regionsX;
	size_t _regionsY;
	float _width;
	float _height;

public:
	RegularGrid();
	~RegularGrid();

	bool initialize(GameMap* map, size_t regionSize, std::vector<GameDynamicObject*> objects);
	void add(GameDynamicObject* entity);
	void remove(GameDynamicObject* entity);
	void update(GameDynamicObject* entity);
	std::vector<GameDynamicObject*> broadphase(GameDynamicObject* entity) const;

private:
	Region & getRegionById(size_t i, size_t j);
	Region& getRegionByCoordinates(int x, int y);
	std::vector<Region&> getRegionsContaining(const Circle& cirlce) const;
	std::vector<Region&> getRegionsContaining(const Segment& segment) const;
	std::vector<Region&> getRegionsForMovement(const Vector2& from, const Vector2& to, float radius) const;
};
