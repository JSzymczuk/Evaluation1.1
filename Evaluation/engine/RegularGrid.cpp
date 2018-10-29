#include "engine/Navigation.h"
#include "engine/RegularGrid.h"
#include "engine/CommonFunctions.h"
#include "math/Math.h"
#include "entities/Entity.h"
#include "entities/Actor.h"
#include "entities/Trigger.h"
#include "engine/Logger.h"



// Fair

//void RegularGrid::requestWriteEnter() {
//	_requestQueueMtx.lock();
//	_accessMtx.lock();
//	_requestQueueMtx.unlock();
//}
//void RegularGrid::requestWriteExit() {
//	_accessMtx.unlock();
//}
//void RegularGrid::requestReadEnter() const {
//	_requestQueueMtx.lock();
//	_readersMtx.lock();
//	if (_currentReaders == 0) {
//		_accessMtx.lock();
//	}
//	++_currentReaders;
//	_requestQueueMtx.unlock();
//	_readersMtx.unlock();
//}
//void RegularGrid::requestReadExit() const {
//	_readersMtx.lock();
//	--_currentReaders;
//	if (_currentReaders == 0) {
//		_accessMtx.unlock();
//	}
//	_readersMtx.unlock();
//}

// Reader preference

void RegularGrid::requestReadEnter() const {
	_readerMtx.lock();
	if (_currentReaders == 0) {
		_writerMtx.lock();
	}
	++_currentReaders;
	_readerMtx.unlock();
}

void RegularGrid::requestReadExit() const {
	_readerMtx.lock();
	--_currentReaders;
	if (_currentReaders == 0) {
		_writerMtx.unlock();
	}
	_readerMtx.unlock();
}

void RegularGrid::requestWriteEnter() {
	_writerMtx.lock();
}

void RegularGrid::requestWriteExit() {
	_writerMtx.unlock();
}


// Writer preference

//void RegularGrid::requestReadEnter() const {
//	_readerRequestMtx.lock();
//	_readerMtx.lock();
//	if (_currentRaders == 0) {
//		_resourceMtx.lock();
//	}
//	++_currentRaders;
//	_readerMtx.unlock();
//	_readerRequestMtx.unlock();
//}
//
//void RegularGrid::requestReadExit() const {
//	_readerMtx.lock();
//	--_currentRaders;
//	if (_currentRaders == 0) {
//		_resourceMtx.unlock();
//	}
//	_readerMtx.unlock();
//}
//
//void RegularGrid::requestWriteEnter() {
//	_writerRequestMtx.lock();
//	if (_currentWriters == 0) {
//		_readerRequestMtx.lock();
//	}
//	++_currentWriters;
//	_writerRequestMtx.unlock();
//
//	_resourceMtx.lock();
//}
//
//void RegularGrid::requestWriteExit() {
//	_resourceMtx.unlock();
//
//	_writerRequestMtx.lock();
//	--_currentWriters;
//	if (_currentWriters == 0) {
//		_readerRequestMtx.unlock();
//	}
//	_writerRequestMtx.unlock();
//}





RegularGrid::RegularGrid(float width, float height, size_t regionSize)
	: CollisionResolver(), _width(width), _height(height), _regions(nullptr), _regionSize(regionSize) { 
	
	_multithreadingEnabled = Config.MultithreadingEnabled;

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

	// powiêksz o maksymalny promieñ obiektu, aby z³apaæ te¿ te, które wystaj¹ z s¹siednich regionów
	radius += Config.ActorRadius;

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

std::vector<RegularGrid::Region*> RegularGrid::getRegionsContaining(const Vector2& point, float radius) {
	std::vector<Region*> result;
	float x = point.x, y = point.y;

	// powiêksz o maksymalny promieñ obiektu, aby z³apaæ te¿ te, które wystaj¹ z s¹siednich regionów
	radius += Config.ActorRadius;

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

std::vector<RegularGrid::Region*> RegularGrid::getRegionsContaining(const Segment& segment) {
	return getRegionsCloseToSegment(segment.from, segment.to, false, 0);
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
			common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
				common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				for (float y = (iyFrom + 1) * _regionSize; y < yTo; y += _regionSize) {
					common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xFrom, y), radius));
				}
				common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
				common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
					common::addIfUnique<const Region*>(result, getRegionsContaining(Vector2(xLast, yLast), radius));
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
			common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
				common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				for (float y = (iyFrom + 1) * _regionSize; y < yTo; y += _regionSize) {
					common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xFrom, y), radius));
				}
				common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
				common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xFrom, yFrom), radius));
				common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xTo, yTo), radius));
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
					common::addIfUnique<Region*>(result, getRegionsContaining(Vector2(xLast, yLast), radius));
				}
				else {
					result.push_back(getRegionById(ix, iy));
				}

			} while ((ix != ixTo || iy != iyTo) && ix >= 0 && iy >= 0 && ix < _regionsX && iy < _regionsY);
		}
	}

	return result;
}

// Operacje na elementach

void RegularGrid::initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) {
	for (DynamicEntity* entity : dynamicObjects) {
		add(entity);
	}
}

void RegularGrid::add(StaticEntity* element) {
	std::vector<Region*> regions;
	for (const Segment& segment : element->getBounds()) {
		common::addIfUnique<Region*>(regions, getRegionsContaining(segment));
	}
	if (_multithreadingEnabled) { requestWriteEnter(); }
	for (Region* region : regions) {
		region->staticObjects.push_back(element);
	}
	if (_multithreadingEnabled) { requestWriteExit(); }
}

void RegularGrid::add(DynamicEntity* element) {
	Vector2 center = element->getPosition();
	Region* region = getRegionByCoordinates(center.x, center.y);
	if (_multithreadingEnabled) { requestWriteEnter(); }
	region->dynamicObjects.push_back(element);
	if (_multithreadingEnabled) { requestWriteExit(); }
	Movable* movable = dynamic_cast<Movable*>(element);
	if (movable != nullptr) { movable->_gridRegion = region; }
	CollisionResolver::addDynamicObject(element);
}

void RegularGrid::remove(DynamicEntity* element) {
	Region* region;
	Movable* movable = dynamic_cast<Movable*>(element);
	if (movable != nullptr) { 
		region = movable->_gridRegion;
		movable->_gridRegion = nullptr;
	}
	else {
		Vector2 center = element->getPosition();
		region = getRegionByCoordinates(center.x, center.y);
	}
	if (_multithreadingEnabled) { requestWriteEnter(); }
	common::swapLastAndRemove(region->dynamicObjects, common::indexOf(region->dynamicObjects, element));
	if (_multithreadingEnabled) { requestWriteExit(); }
	CollisionResolver::removeDynamicObject(element);
}

void RegularGrid::update(Movable* element) {
	if (element->hasPositionChanged()) {
		Vector2 position = element->getPosition();
		Region* newRegion = getRegionByCoordinates(position.x, position.y);
		Region* oldRegion = element->_gridRegion;
		if (oldRegion != newRegion) {
			if (_multithreadingEnabled) { requestWriteEnter(); }
			common::swapLastAndRemove(oldRegion->dynamicObjects, common::indexOf<DynamicEntity*>(oldRegion->dynamicObjects, element));
			newRegion->dynamicObjects.push_back(element);
			if (_multithreadingEnabled) { requestWriteExit(); }
			element->_gridRegion = newRegion;
		}
	}
}

// Faza ogólna

std::vector<DynamicEntity*> RegularGrid::broadphaseDynamic(const Vector2& point, float radius) const {
	std::vector<DynamicEntity*> result;

	if (_multithreadingEnabled) { 
		requestReadEnter();
	}

	auto regions = getRegionsContaining(point, radius);

	for (const Region* region : regions) {
		common::merge(result, region->dynamicObjects);
	}

	if (_multithreadingEnabled) {
		requestReadExit();
	}

	return result;
}

std::vector<DynamicEntity*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to) const {
	std::vector<DynamicEntity*> result;

	if (_multithreadingEnabled) {
		requestReadEnter();
	}

	auto regions = getRegionsForMovement(from, to, Config.ActorRadius);

	for (const Region* region : regions) {
		common::merge(result, region->dynamicObjects);
	}

	if (_multithreadingEnabled) {
		requestReadExit();
	}

	return result;
}

std::vector<DynamicEntity*> RegularGrid::broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const {
	std::vector<DynamicEntity*> result;

	if (_multithreadingEnabled) {
		requestReadEnter();
	}

	auto regions = getRegionsForMovement(from, to, radius + Config.ActorRadius);

	for (const Region* region : regions) {
		common::merge(result, region->dynamicObjects);
	}

	if (_multithreadingEnabled) {
		requestReadExit();
	}

	return result;
}

std::vector<StaticEntity*> RegularGrid::broadphaseStatic(const Vector2& point, float radius) const {
	std::vector<StaticEntity*> result;

	auto regions = getRegionsContaining(point, radius);

	for (const Region* region : regions) {
		common::addIfUnique<StaticEntity*>(result, region->staticObjects);
	}

	return result;
}

std::vector<StaticEntity*> RegularGrid::broadphaseStatic(const Vector2& from, const Vector2& to) const {
	std::vector<StaticEntity*> result;

	auto regions = getRegionsContaining(Segment(from, to));

	for (const Region* region : regions) {
		if (region->staticObjects.size() > 0) {
			common::addIfUnique<StaticEntity*>(result, region->staticObjects);
		}
	}

	return result;
}

std::vector<StaticEntity*> RegularGrid::broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const {
	std::vector<StaticEntity*> result;

	auto regions = getRegionsForMovement(from, to, radius);

	for (const Region* region : regions) {
		common::addIfUnique<StaticEntity*>(result, region->staticObjects);
	}

	return result;
}
