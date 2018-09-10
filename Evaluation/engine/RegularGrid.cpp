#include "engine/Navigation.h"
#include "engine/RegularGrid.h"
#include "engine/CommonFunctions.h"

RegularGrid::RegularGrid() { _regions = nullptr; }
RegularGrid::~RegularGrid() {
	for (size_t i = 0; i < _regionsX; ++i) {
		delete[] _regions[i];
	}
	delete[] _regions;
}

bool RegularGrid::initialize(GameMap* map, size_t regionSize, std::vector<GameDynamicObject*> objects) {
	if (_regions != nullptr || regionSize == 0) { return false; }

	_width = map->getWidth();
	_height = map->getHeight();
	_regionSize = regionSize;
	_regionsX = (size_t)ceil(_width / _regionSize);
	_regionsY = (size_t)ceil(_height / _regionSize);

	_regions = new Region*[_regionsX];
	for (size_t i = 0; i < _regionsX; ++i) {
		_regions[i] = new Region[_regionsY];
	}

	return true;
}
