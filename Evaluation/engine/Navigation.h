#pragma once

#include <algorithm>
#include <fstream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#include "AabbTree.h"
#include "Rng.h"
#include "SegmentTree.h"
#include "main/Configuration.h"

class GameDynamicObject;
class Wall;

class GameMap {
public:
	static GameMap* create(const char* filepath);
	static void destroy(GameMap* game);
	std::vector<GameDynamicObject*> initializeEntities(const std::vector<GameDynamicObject*>& entities);

	std::vector<GameDynamicObject*> getEntities() const;
	std::vector<Wall> getWalls() const;

	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, GameDynamicObject* movable) const;
	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, GameDynamicObject* movable, const std::vector<common::Circle>& ignoredAreas) const;
	bool raycastStatic(const Segment& ray, Vector2& result) const;
	std::vector<GameDynamicObject*> checkCollision(const Vector2& point) const;
	std::vector<GameDynamicObject*> checkCollision(const Aabb& area) const;
	bool isMovementValid(GameDynamicObject* movable, const Vector2& movementVector) const;
	bool isPositionValid(const Vector2& point, float entityRadius) const;

#ifdef _DEBUG
	Vector2 getClosest(const Vector2& point) const;
	std::vector<Vector2> getNavigationNodes() const;
	std::vector<Segment> getNavigationArcs() const;
	std::vector<Aabb> getAabbs() const;
#endif

private:
	struct NavigationNode {

		Vector2 position;
		int index;
		typedef std::pair<int, float> Arc;
		std::vector<Arc> arcs;

		NavigationNode(float x, float y, int index);
	};

	std::vector<NavigationNode> _navigationMesh;
	SegmentTree<Wall> _walls;
	AabbTree<GameDynamicObject*> _entities;
	float _width;
	float _height;

	int getClosestNavigationNode(const Vector2& point, const std::vector<common::Circle>& ignoredAreas) const;
	std::vector<int> aStar(int from, int to, const std::vector<common::Circle>& ignoredAreas) const;
	float estimateDistance(int fromIdx, int toIdx) const;
	Vector2 getNodePosition(int index) const;

	static const int NULL_IDX;

public:
	class Loader {
	public:
		GameMap* load(const char* mapFilename);
#ifdef _DEBUG
		//void generateConnections(const String& inputFile, const String& outputFile);
#endif

	private:
		void loadMapSize();
		void loadNavigationPoints();
		void loadNavigationMesh();
		void loadTriggers();

		//void appendIfNotContains(std::vector<GameMap::NavigationNode>& collection, const Vector2& point) const;
		
		GameMap* _map;
		std::ifstream _reader;
	};
};
