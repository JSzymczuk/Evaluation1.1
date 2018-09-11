#include "engine/Navigation.h"
#include "engine/RegularGrid.h"
#include "engine/CommonFunctions.h"
#include "math/Math.h"
#include "entities/Entity.h"

RegularGrid::RegularGrid() { _regions = nullptr; }
RegularGrid::~RegularGrid() {
	for (size_t i = 0; i < _regionsX; ++i) {
		delete[] _regions[i];
	}
	delete[] _regions;
}

std::vector<GameDynamicObject*> RegularGrid::initialize(GameMap* map, size_t regionSize, std::vector<GameDynamicObject*> objects) {
	std::vector<GameDynamicObject*> invalid;
	
	if (_regions != nullptr || regionSize == 0) { return invalid; }

	_width = map->getWidth();
	_height = map->getHeight();
	_regionSize = regionSize;
	_regionsX = (size_t)ceil(_width / _regionSize);
	_regionsY = (size_t)ceil(_height / _regionSize);

	_regions = new Region*[_regionsX];
	for (size_t i = 0; i < _regionsX; ++i) {
		_regions[i] = new Region[_regionsY];
		for (size_t j = 0; j < _regionsY; ++j) {
			_regions[i][j].idX = i;
			_regions[i][j].idY = j;
		}
	}

	for (GameDynamicObject* entity : objects) {
		add(entity);
	}

	return invalid;
}

RegularGrid::Region* RegularGrid::getRegionById(size_t i, size_t j) {
	if (i < _regionsX && j < _regionsY) {
		return &_regions[i][j];
	}
	return nullptr;
}

RegularGrid::Region* RegularGrid::getRegionByCoordinates(float x, float y) {
	if (0 <= x && x < _width && 0 <= y && y < _height) {
		size_t i = x / _regionSize;
		size_t j = y / _regionSize;
		return &_regions[i][j];
	}
	return nullptr;
}

std::vector<RegularGrid::Region*> RegularGrid::getRegionsContaining(const Vector2& point, float radius) {
	std::vector<Region*> result;	
	
	float x = point.x, y = point.y;
	Region* region = getRegionByCoordinates(x, y);
	if (region == nullptr) { return result; }

	result.push_back(region);
	size_t idX = region->idX, idY = region->idY;

	float xMin = region->idX * _regionSize, xMax = xMin + _regionSize;
	float yMin = region->idY * _regionSize, yMax = yMin + _regionSize;

	if (x - radius <= xMin && idX > 0) {
		result.push_back(getRegionById(idX - 1, idY));
		if (idY > 0 && common::sqr(x - xMin) + common::sqr(y - yMin) <= radius * radius) {
			result.push_back(getRegionById(idX - 1, idY - 1));
		}
		if (idY < _regionsY - 1 && common::sqr(x - xMin) + common::sqr(y - yMax) <= radius * radius) {
			result.push_back(getRegionById(idX - 1, idY + 1));
		}
	}
	if (x + radius >= xMax && idX < _regionsX - 1) {
		result.push_back(getRegionById(idX + 1, idY));
		if (idY > 0 && common::sqr(x - xMax) + common::sqr(y - yMin) <= radius * radius) {
			result.push_back(getRegionById(idX + 1, idY - 1));
		}
		if (idY < _regionsY - 1 && common::sqr(x - xMax) + common::sqr(y - yMax) <= radius * radius) {
			result.push_back(getRegionById(idX + 1, idY + 1));
		}
	}
	if (y - radius <= yMin && idY > 0) {
		result.push_back(getRegionById(idX, idY - 1));
	}
	if (y + radius >= yMax && idY < _regionsY - 1) {
		result.push_back(getRegionById(idX, idY + 1));
	}

	return result;
}

std::vector<RegularGrid::Region*> RegularGrid::getRegionsContaining(const Segment& segment) {
	return getRegionsCloseToSegment(segment.from, segment.to, false, 0);
}

std::vector<RegularGrid::Region*> RegularGrid::getRegionsForMovement(const Vector2& from, const Vector2& to, float radius) {
	return getRegionsCloseToSegment(from, to, true, radius);
}

std::vector<RegularGrid::Region*> RegularGrid::getRegionsCloseToSegment(
	const Vector2& from, const Vector2& to, bool useRadius, float radius) {

	std::vector<Region*> result;

	float xFrom = from.x; 
	float yFrom = from.y;
	float xTo = to.x;
	float yTo = to.y;

	int ixFrom = xFrom / _regionSize;
	int iyFrom = yFrom / _regionSize;
	int ixTo = xTo / _regionSize;
	int iyTo = yTo / _regionSize;

	if (ixFrom == ixTo && iyFrom == iyTo) {
		if (useRadius) {
			result = getRegionsContaining(Vector2(xFrom, yFrom), radius);
			common::addIfUnique(result, getRegionsContaining(Vector2(xTo, yTo), radius));
		}
		else {
			result.push_back(getRegionById(ixFrom, iyFrom));
		}
	}
	else {
		if (common::abs(xFrom - xTo) <= common::EPSILON) {
			// Przypadek degneruje siê do linii pionowej
			if (yFrom > yTo) {
				common::swap(yFrom, yTo);
				common::swap(iyFrom, iyTo);
			}

			if (useRadius) {
				common::addIfUnique(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				for (float y = (iyFrom + 1) * _regionSize; y < yTo; y += _regionSize) {
					common::addIfUnique(result, getRegionsContaining(Vector2(xFrom, y), radius));
				}
				common::addIfUnique(result, getRegionsContaining(Vector2(xTo, yTo), radius));
			}
			else {
				for (size_t iy = iyFrom; iy <= iyTo; ++iy) {
					result.push_back(getRegionById(ixFrom, iy));
				}
			}
		}
		else {
			if (xFrom > xTo) {
				common::swap(xFrom, xTo);
				common::swap(yFrom, yTo);
				common::swap(ixFrom, ixTo);
				common::swap(iyFrom, iyTo);
			}

			float d = (yTo - yFrom) / (xTo - xFrom);
			int ix = ixFrom, iy = iyFrom;

			if (useRadius) {
				common::addIfUnique(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				common::addIfUnique(result, getRegionsContaining(Vector2(xTo, yTo), radius));
			}
			else {
				result.push_back(getRegionById(ix, iy));
			}

			// Wspó³rzêdne ostatniego punktu (przeciêcia lub koñcowego)
			float xLast = xFrom, yLast = yFrom;

			float xRegionEnd = (ix + 1) * _regionSize;
			float yRegionStart = iy * _regionSize;
			float yRegionEnd = yRegionStart + _regionSize;

			// Wspó³rzêdna y nastêpnego punktu przeciêcia z prawym koñcem regionu
			float yNext = yLast + d * (xRegionEnd - xLast);

			do {
				// Okreœlamy, na której krawêdzi regionu znajduje siê nastêpny punkt przeciêcia
				if (yRegionStart <= yNext && yNext < yRegionEnd) {
					// Przesuwamy siê w prawo
					++ix;
					xLast = xRegionEnd;
					yLast = yNext;
					xRegionEnd += _regionSize;
					yNext = yLast + d * _regionSize;
				}
				else {
					if (yNext < yRegionStart) {
						// Do góry
						xLast += (yRegionStart - yLast) / d;
						--iy;
						yLast = yRegionStart;
						yRegionEnd = yRegionStart;
						yRegionStart -= _regionSize;
					}
					else {
						// Do do³u
						xLast += (yRegionEnd - yLast) / d;
						++iy;
						yLast = yRegionEnd;
						yRegionStart = yRegionEnd;
						yRegionEnd += _regionSize;
					}
				}

				if (useRadius) {
					common::addIfUnique(result, getRegionsContaining(Vector2(xLast, yLast), radius));
				}
				else {
					result.push_back(getRegionById(ix, iy));
				}

			} while (ix != ixTo || iy != iyTo); 
		}
	}

	return result;
}

void RegularGrid::add(GameDynamicObject* entity) {
	common::Circle collisionShape = entity->getCollisionArea();
	for (Region* region : getRegionsContaining(collisionShape.center, collisionShape.radius)) {
		region->dynamicObjects.push_back(entity);
	}
}

void RegularGrid::remove(GameDynamicObject* entity) {
	common::Circle collisionShape = entity->getCollisionArea();
	for (Region* region : getRegionsContaining(collisionShape.center, collisionShape.radius)) {
		common::swapLastAndRemove(region->dynamicObjects, common::indexOf(region->dynamicObjects, entity));
	}
}

template <typename T> void uniqueElements(const std::vector<T>& first, const std::vector<T>& second,
	std::vector<T>& uniqueFirst, std::vector<T>& uniqueSecond) {

	for (T t : first) {
		if (!common::contains(second, t)) { uniqueFirst.push_back(t); }
	}
	for (T t : second) {
		if (!common::contains(first, t)) { uniqueSecond.push_back(t); }
	}
}

void RegularGrid::update(GameDynamicObject* entity, const Vector2& previousPosition) {
	float r = entity->getRadius();
	std::vector<Region*> toRemove;
	std::vector<Region*> toAdd;
	uniqueElements(getRegionsContaining(previousPosition, r), getRegionsContaining(entity->getPosition(), r), toRemove, toAdd);
	for (Region* region : toAdd) {
		region->dynamicObjects.push_back(entity);
	}
	for (Region* region : toRemove) {
		common::swapLastAndRemove(region->dynamicObjects, common::indexOf(region->dynamicObjects, entity));
	}
}

std::vector<GameDynamicObject*> RegularGrid::broadphase(const Vector2& point, float radius) {
	std::vector<GameDynamicObject*> result;
	for (Region* region : getRegionsContaining(point, radius)) {
		common::addIfUnique(result, region->dynamicObjects);
	}
	return result;
}
