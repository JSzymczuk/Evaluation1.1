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

class DynamicEntity;
class Actor;
class Trigger;
class Wall;

class GameMap {
public:
	static GameMap* create(const char* filepath);
	static void generateConnections(const String& inputFile, const String& outputFile);
	static void destroy(GameMap* game);

	float getWidth() const;
	float getHeight() const;

	std::vector<Actor*> getActors() const;
	std::vector<Trigger*> getTriggers() const;
	std::vector<StaticEntity*> getWalls() const;

	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, Movable* movable) const;
	std::queue<Vector2> findPath(const Vector2& from, const Vector2& to, Movable* movable, const std::vector<common::Circle>& ignoredAreas) const;
	bool raycastStatic(const Segment& ray, Vector2& result) const;

	bool canPlace(const DynamicEntity* object) const;
	bool place(Actor* actor);
	bool place(Trigger* trigger);
	void remove(Actor* actor);

	std::vector<DynamicEntity*> checkCollision(const Vector2& point, float radius);
	std::vector<DynamicEntity*> checkCollision(const Segment& segment);
	std::vector<Destructible*> checkCollisionDestructible(const Vector2& point, float radius) const;

	const CollisionResolver* getCollisionResolver() const;

	void initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects);


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
	CollisionResolver* _collisionResolver;
	std::vector<Trigger*> _triggers;
	std::vector<Actor*> _entities;
	std::vector<StaticEntity*> _walls;

	int getClosestNavigationNode(const Vector2& point, const std::vector<common::Circle>& ignoredAreas) const;
	std::vector<int> aStar(int from, int to, const std::vector<common::Circle>& ignoredAreas) const;
	float estimateDistance(int fromIdx, int toIdx) const;
	Vector2 getNodePosition(int index) const;

	static const int NULL_IDX;


	class Loader {
	public:
		GameMap* load(const char* mapFilename);
		void generateConnections(const String& inputFile, const String& outputFile); 

	private:
		void loadMapSize();
		void loadNavigationPoints();
		void loadNavigationMesh();
		std::vector<StaticEntity*> loadStaticObjects();
		std::vector<Trigger*> loadTriggers();
		
		GameMap* _map;
		std::ifstream _reader;
	};
};
