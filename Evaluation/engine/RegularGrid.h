#pragma once

#include <mutex>
#include <shared_mutex>
#include "engine/CollisionResolver.h"

class RegularGrid : public CollisionResolver {
public:
	RegularGrid(float width, float height, size_t regionSize);
	~RegularGrid();

	void initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) override;
	
	void add(StaticEntity* element) override;
	void add(DynamicEntity* element) override;
	void remove(DynamicEntity* element) override;
	void update(Movable* element) override;
			
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& point, float radius) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to) const override;
	std::vector<DynamicEntity*> broadphaseDynamic(const Vector2& from, const Vector2& to, float radius) const override;

	std::vector<StaticEntity*> broadphaseStatic(const Vector2& point, float radius) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to) const override;
	std::vector<StaticEntity*> broadphaseStatic(const Vector2& from, const Vector2& to, float radius) const override;

	struct Region {
		size_t idX;
		size_t idY;
		std::vector<StaticEntity*> staticObjects;
		std::vector<DynamicEntity*> dynamicObjects;
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
	bool _multithreadingEnabled;
	Region* getRegionById(size_t i, size_t j);
	Region* getRegionByCoordinates(float x, float y);
	std::vector<Region*> getRegionsContaining(const Vector2& point, float radius);
	std::vector<Region*> getRegionsContaining(const Segment& segment);

	std::vector<Region*> getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius);
	std::vector<const Region*> getRegionsCloseToSegment(const Vector2& from, const Vector2& to, bool useRadius, float radius) const;

	friend class Movable;

	void requestWriteEnter();
	void requestWriteExit();
	void requestReadEnter() const;
	void requestReadExit() const;

	// Fair reader-writex problem	
	//mutable size_t _currentReaders = 0;
	//mutable std::shared_mutex _requestQueueMtx;
	//mutable std::shared_mutex _accessMtx;
	//mutable std::mutex _readersMtx;

	// Reader preference
	mutable size_t _currentReaders = 0;
	mutable std::mutex _readerMtx;
	mutable std::shared_mutex _writerMtx;

	// Writer preference
	/*mutable size_t _currentRaders = 0;
	mutable size_t _currentWriters = 0;
	mutable std::mutex _readerMtx;
	mutable std::mutex _writerRequestMtx;
	mutable std::shared_mutex _readerRequestMtx;
	mutable std::shared_mutex _resourceMtx;*/
};
