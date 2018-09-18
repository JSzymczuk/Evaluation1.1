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
#include "engine/RegularGrid.h"

class GameDynamicObject;
class Actor;
class Trigger;
class Wall;

class GameMap {
public:
	static GameMap* create(const char* filepath);
	static void destroy(GameMap* game);

	float getWidth() const;
	float getHeight() const;

	std::vector<Actor*> getActors() const;
	std::vector<Trigger*> getTriggers() const;
	std::vector<GameStaticObject*> getWalls() const;

	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, GameDynamicObject* movable) const;
	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, GameDynamicObject* movable, const std::vector<common::Circle>& ignoredAreas) const;
	bool raycastStatic(const Segment& ray, Vector2& result) const;
	std::vector<GameDynamicObject*> checkCollision(const Vector2& point);
	std::vector<GameDynamicObject*> checkCollision(const Vector2& point, float radius);
	std::vector<GameDynamicObject*> checkCollision(const Segment& segment);
	//std::vector<GameDynamicObject*> checkCollision(const Aabb& area) const;
	bool isMovementValid(GameDynamicObject* movable, const Vector2& movementVector) const;
	bool isPositionValid(const Vector2& point, float entityRadius) const;
	bool canPlace(const GameDynamicObject* object) const;
	bool place(GameDynamicObject* object);
	void remove(Actor* object);

#ifdef _DEBUG
	Vector2 getClosest(const Vector2& point) const;
	std::vector<Vector2> getNavigationNodes() const;
	std::vector<Segment> getNavigationArcs() const;
#endif

private:
	struct NavigationNode {

		Vector2 position;
		int index;
		typedef std::pair<int, float> Arc;
		std::vector<Arc> arcs;

		NavigationNode(float x, float y, int index);
	};

	float _width;
	float _height;
	std::vector<NavigationNode> _navigationMesh;
public:
	CollisionResolver* _collisionResolver;
private:
	std::vector<Trigger*> _triggers;
	std::vector<Actor*> _entities;
	std::vector<GameStaticObject*> _walls;

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
		std::vector<GameStaticObject*> loadStaticObjects();
		
		GameMap* _map;
		std::ifstream _reader;
	};
};
