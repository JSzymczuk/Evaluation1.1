#include "engine/Navigation.h"
#include "engine/RegularGrid.h"
#include "engine/CommonFunctions.h"
#include "math/Math.h"
#include "entities/Entity.h"
#include "entities/Actor.h"
#include "engine/Logger.h"

RegularGrid::RegularGrid(float width, float height, size_t regionSize)
	: _width(width), _height(height), _regions(nullptr), _regionSize(regionSize) { 

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
}

RegularGrid::~RegularGrid() {
	for (size_t i = 0; i < _regionsX; ++i) {
		delete[] _regions[i];
	}
	delete[] _regions;
}

bool RegularGrid::isPositionValid(const GameDynamicObject* entity) const {
	Vector2 position = entity->getPosition();
	float radius = entity->getRadius();
	return position.x > radius && position.y > radius && position.x < _width - radius && position.y < _height - radius 
		&& narrowphaseDynamic(entity).size() == 0 && narrowphaseStatic(entity).size() == 0;
}

RegularGrid::Region* RegularGrid::getRegionById(size_t i, size_t j) {
	if (i < _regionsX && j < _regionsY) {
		return &_regions[i][j];
	}
	return nullptr;
}

const RegularGrid::Region* RegularGrid::getRegionById(size_t i, size_t j) const {
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

const RegularGrid::Region* RegularGrid::getRegionByCoordinates(float x, float y) const {
	if (0 <= x && x < _width && 0 <= y && y < _height) {
		size_t i = x / _regionSize;
		size_t j = y / _regionSize;
		return &_regions[i][j];
	}
	return nullptr;
}

std::vector<const RegularGrid::Region*> RegularGrid::getRegionsContaining(const Vector2& point, float radius) const {
	std::vector<const Region*> result;	
	float x = point.x, y = point.y;

	int ixFrom = (x - radius) / _regionSize, iyFrom = (y - radius) / _regionSize;
	int ixTo = (x + radius) / _regionSize, iyTo = (y + radius) / _regionSize;

	if (ixFrom < 0) { ixFrom = 0; }
	if (iyFrom < 0) { iyFrom = 0; }
	if (ixTo >= _regionsX) { ixTo = _regionsX - 1; }
	if (iyTo >= _regionsY) { iyTo = _regionsY - 1; }

	for (int iy = iyFrom; iy <= iyTo; ++iy) {
		for (int ix = ixFrom; ix <= ixTo; ++ix) {
			result.push_back(&_regions[ix][iy]);
		}
	}

	return result;
}

std::vector<const RegularGrid::Region*> RegularGrid::getRegionsContaining(const Segment& segment) const {
	return getRegionsCloseToSegment(segment.from, segment.to, false, 0);
}

std::vector<const RegularGrid::Region*> RegularGrid::getRegionsForMovement(const Vector2& from, const Vector2& to, float radius) const {
	return getRegionsCloseToSegment(from, to, true, radius);
}

std::vector<const RegularGrid::Region*> RegularGrid::getRegionsCloseToSegment(
	const Vector2& from, const Vector2& to, bool useRadius, float radius) const {

	std::vector<const Region*> result;

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
			if (0 <= ixFrom && ixFrom < _regionsX && 0 <= iyFrom && iyFrom < _regionsY) {
				result.push_back(getRegionById(ixFrom, iyFrom));
			}
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
				if (0 <= ixFrom && ixFrom < _regionsX) {
					for (size_t iy = iyFrom; iy < _regionsY && iy <= iyTo; ++iy) {
						result.push_back(getRegionById(ixFrom, iy));
					}
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
				const Region* region = getRegionById(ix, iy);
				if (region != nullptr) {
					result.push_back(region);
				}
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
					const Region* region = getRegionById(ix, iy);
					if (region != nullptr) {
						result.push_back(region);
					}
				}

			} while ((ix != ixTo || iy != iyTo) && ix >= 0 && iy >= 0 && ix < _regionsX && iy < _regionsY); 
		}
	}

	return result;
}

std::vector<RegularGrid::Region*> RegularGrid::getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius) {
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

			} while ((ix != ixTo || iy != iyTo) && ix >= 0 && iy >= 0 && ix < _regionsX && iy < _regionsY);
		}
	}

	return result;
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

void RegularGrid::add(GameDynamicObject* element) {
	common::Circle collisionShape = element->getCollisionArea();
	_mtx.lock(); 
	for (Region* region : getRegionsContaining(collisionShape.center, collisionShape.radius)) {
		region->dynamicObjects.push_back(element);
	}
	_mtx.unlock();
	element->enableCollisions(this);
}

void RegularGrid::remove(GameDynamicObject* element) {
	common::Circle collisionShape = element->getCollisionArea();
	_mtx.lock(); 
	for (Region* region : getRegionsContaining(collisionShape.center, collisionShape.radius)) {
		common::swapLastAndRemove(region->dynamicObjects, common::indexOf(region->dynamicObjects, element));
	}
	_mtx.unlock();
	element->disableCollisions();
}

void RegularGrid::update(GameDynamicObject* element) {
	if (element->hasPositionChanged()) {
		float r = element->getRadius();
		Vector2 position = element->getPosition();
		Vector2 previousPosition = position - element->getVelocity();
		std::vector<Region*> toRemove;
		std::vector<Region*> toAdd;
		uniqueElements(getRegionsContaining(previousPosition, r), getRegionsContaining(position, r), toRemove, toAdd);
		_mtx.lock();
		for (Region* region : toAdd) {
			region->dynamicObjects.push_back(element);
		}
		for (Region* region : toRemove) {
			common::swapLastAndRemove(region->dynamicObjects, common::indexOf(region->dynamicObjects, element));
		}
		_mtx.unlock();
	}
}

void RegularGrid::add(GameStaticObject* element) {
	std::vector<Region*> regions;
	for (const Segment& segment : element->getBounds()) {
		common::addIfUnique(regions, getRegionsContaining(segment));
	}
	_mtx.lock();
	for (Region* region : regions) {
		region->staticObjects.push_back(element);
	}
	_mtx.unlock();
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& point, float radius, std::vector<std::pair<size_t, size_t>>& regions) const {
	std::vector<GameDynamicObject*> result;
	_mtx.lock();
	for (const Region* region : getRegionsContaining(point, radius)) {
		common::addIfUnique(result, region->dynamicObjects);
		regions.push_back(std::make_pair(region->idX, region->idY));
	}
	_mtx.unlock();
	return result;
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to, std::vector<std::pair<size_t, size_t>>& regions) const {
	std::vector<GameDynamicObject*> result;
	_mtx.lock();
	for (const Region* region : getRegionsContaining(Segment(from, to))) {
		common::addIfUnique(result, region->dynamicObjects);
		regions.push_back(std::make_pair(region->idX, region->idY));
	}
	_mtx.unlock();
	return result;
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to, float radius, std::vector<std::pair<size_t, size_t>>& regions) const {
	std::vector<GameDynamicObject*> result;
	_mtx.lock();
	for (const Region* region : getRegionsForMovement(from, to, radius)) {
		common::addIfUnique(result, region->dynamicObjects);
		regions.push_back(std::make_pair(region->idX, region->idY));
	}
	_mtx.unlock();
	return result;
}

std::vector<GameStaticObject*> RegularGrid::broadphaseStatic(const Vector2& point, float radius) const {
	std::vector<GameStaticObject*> result;
	for (const Region* region : getRegionsContaining(point, radius)) {
		common::addIfUnique(result, region->staticObjects);
	}
	return result;
}

std::vector<GameStaticObject*> RegularGrid::broadphaseStatic(const Vector2& from, const Vector2& to) const {
	std::vector<GameStaticObject*> result;
	for (const Region* region : getRegionsContaining(Segment(from, to))) {
		if (region->staticObjects.size() > 0) {
			common::addIfUnique(result, region->staticObjects);
		}
	}
	return result;
}

std::vector<GameStaticObject*> RegularGrid::broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const {
	std::vector<GameStaticObject*> result;
	for (const Region* region : getRegionsForMovement(from, to, radius)) {
		common::addIfUnique(result, region->staticObjects);
	}
	return result;
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

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& point) const {
	std::vector<GameDynamicObject*> result;
	_mtx.lock();
	result = getRegionByCoordinates(point.x, point.y)->dynamicObjects;
	_mtx.unlock();
	return result;
}

std::vector<GameStaticObject*> RegularGrid::broadphaseStatic(const Vector2& point) const {
	std::vector<GameStaticObject*> result;
	_mtx.lock();
	result = getRegionByCoordinates(point.x, point.y)->staticObjects;
	_mtx.unlock();
	return result;
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& point, float radius) const {
	std::vector<std::pair<size_t, size_t>> regions;
	return broadphaseDynamic(point, radius, regions);
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to) const {
	std::vector<std::pair<size_t, size_t>> regions;
	return broadphaseDynamic(from, to, regions);
}

std::vector<GameDynamicObject*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const {
	std::vector<std::pair<size_t, size_t>> regions;
	return broadphaseDynamic(from, to, radius, regions);
}

std::vector<GameDynamicObject*> RegularGrid::narrowphaseDynamic(const GameDynamicObject* entity) const {
	std::vector<GameDynamicObject*> result;
	Vector2 position = entity->getPosition();
	float radius = entity->getRadius() + Config.MovementSafetyMargin, r2 = radius * radius;
	for (GameDynamicObject* other : broadphaseDynamic(position, radius)) {
		if (entity != other && common::sqDist(position, other->getPosition()) <= r2) {
			result.push_back(other);
		}
	}
	return result;
}

std::vector<GameStaticObject*> RegularGrid::narrowphaseStatic(const GameDynamicObject* entity) const {
	std::vector<GameStaticObject*> result;
	Vector2 position = entity->getPosition();
	float radius = entity->getRadius() + Config.MovementSafetyMargin;
	for (GameStaticObject* other : broadphaseStatic(position, radius)) {
		if (other->getDistanceTo(position) <= radius) {
			result.push_back(other);
		}
	}
	return result;
}
