#include "Navigation.h"

#include "entities/Entity.h"
#include "entities/Wall.h"
#include "engine/SegmentTree.h"
#include "main/Configuration.h"


float GameMap::getWidth() const { return _width; }
float GameMap::getHeight() const { return _height; }

GameMap* GameMap::create(const char* filepath) { return Loader().load(filepath); }

void GameMap::destroy(GameMap* game) { 
	std::vector<GameDynamicObject*> entities = game->getEntities();
	for (auto entityPtr : entities) {
		delete entityPtr;
	}
	delete game;
}

std::vector<GameStaticObject*> GameMap::getWalls() const { return _walls; }

bool GameMap::raycastStatic(const Segment& ray, Vector2& result) const {
	Vector2 rayOrigin = ray.from;
	bool collisionFound = false;
	float minDist;

	for (GameStaticObject* staticObj : _collisionResolver->broadphaseStatic(ray.from, ray.to)) {
		Vector2 collisionResult;
		for (const Segment& seg : staticObj->getBounds()) {
			if (common::testSegments(ray, seg, collisionResult)) {
				float dist = common::sqDist(rayOrigin, collisionResult);
				if (!collisionFound || dist < minDist) {
					collisionFound = true;
					minDist = dist;
					result = collisionResult;
				}
			}
		}
	}

	return collisionFound;
}

const int GameMap::NULL_IDX = -1;

GameMap::NavigationNode::NavigationNode(float x, float y, int index) : position(x, y), index(index) { }

std::vector<GameDynamicObject*> GameMap::initializeEntities(const std::vector<GameDynamicObject*>& entities) {
	auto initializeResult = _collisionResolver->initialize(entities);
	_entities = initializeResult.first;
	return initializeResult.second;
}

int GameMap::getClosestNavigationNode(const Vector2& point, const std::vector<common::Circle>& ignoredAreas) const {
	if (_navigationMesh.size() == 0) { return NULL_IDX; }

	int closest = NULL_IDX;
	Vector2 tempVector;
	float minDist = std::numeric_limits<float>::infinity();

	for (NavigationNode node : _navigationMesh) {
		bool isAllowed = true;
		for (const common::Circle& circle : ignoredAreas) {
			if (circle.contains(node.position)) {
				isAllowed = false;
				break;
			}
		}
		if (isAllowed) {
			float dist = (node.position - point).lengthSquared();
			if (node.index != NULL_IDX && dist < minDist
				// Promieñ nie powoduje przeciêcia ze œcian¹ lub przeciêcie jest punktem nawigacji
				&& (!raycastStatic(Segment(point, node.position), tempVector)
					|| common::sqDist(tempVector, node.position) < common::EPSILON)) {
				minDist = dist;
				closest = node.index;
			}
		}
	}

	return closest;
}

std::vector<GameDynamicObject*> GameMap::checkCollision(const Vector2& point) { 
	return _collisionResolver->broadphaseDynamic(point);
}

//std::vector<GameDynamicObject*> GameMap::checkCollision(const Aabb& area) const { 
//	return _collisionResolver->broadphase(area);
//}

std::vector<GameDynamicObject*> GameMap::checkCollision(const Vector2& point, float radius) {
	return _collisionResolver->broadphaseDynamic(point, radius);
}

std::vector<GameDynamicObject*> GameMap::checkCollision(const Segment& segment) {
	return _collisionResolver->broadphaseDynamic(segment.from, segment.to);
}

std::vector<GameDynamicObject*> GameMap::getEntities() const {
	return _entities;
}

struct AStarNodeInfo {
	int nodeValue;
	int previous;
	float costSoFar;
	float estimatedTotalCost;
};

bool lowerCost2(const AStarNodeInfo& first, const AStarNodeInfo& second) {
	return first.estimatedTotalCost > second.estimatedTotalCost;
}

float GameMap::estimateDistance(int fromIdx, int toIdx) const {
	return common::distance(_navigationMesh.at(fromIdx).position, _navigationMesh.at(toIdx).position);
}

typedef std::vector<AStarNodeInfo> AStarCollection;

AStarCollection::iterator findNode(AStarCollection& collection, int value) {
	auto it = collection.begin();
	for (; it != collection.end(); ++it) {
		if (it->nodeValue == value) { break; }
	}
	return it;
}

AStarCollection::const_iterator findNode(const AStarCollection& collection, int value) {
	auto it = collection.begin();
	for (; it != collection.end(); ++it) {
		if (it->nodeValue == value) { break; }
	}
	return it;
}

bool containsNode(const AStarCollection& collection, int value) {
	return findNode(collection, value) != collection.end();
}

bool containsNode(const AStarCollection& collection, int value, AStarNodeInfo& result) {
	auto iter = findNode(collection, value);
	if (iter != collection.end()) {
		result = *iter;
		return true;
	}
	return false;
}

void removeNodeAt(AStarCollection& collection, unsigned int index) {
	unsigned int n = collection.size();
	if (n > 0 && index < n) {
		collection.at(index) = collection.at(n - 1);
		collection.pop_back();
	}
}

void removeNode(AStarCollection& collection, int value) {
	auto it = findNode(collection, value);
	if (it != collection.end()) {
		auto last = collection.at(collection.size() - 1);
		(*it) = last;
		collection.pop_back();
	}
}

std::vector<int> GameMap::aStar(int from, int to, const std::vector<common::Circle>& ignoredAreas) const {

	std::map<int, bool> allowedNodes;

	AStarNodeInfo startRecord;
	startRecord.nodeValue = from;
	startRecord.costSoFar = 0;
	startRecord.estimatedTotalCost = estimateDistance(from, to);

	std::vector<AStarNodeInfo> open;
	std::vector<AStarNodeInfo> closed;
	open.push_back(startRecord);
	make_heap(open.begin(), open.end(), lowerCost2);

	AStarNodeInfo current;

	while (open.size() > 0) {

		current = open.front();
		pop_heap(open.begin(), open.end(), lowerCost2);
		open.pop_back();

		if (current.nodeValue == to) { break; }
		int currentNodeIdx = current.nodeValue;

		// Rejestrujemy, które wêz³y bêd¹ ignorowane. 
		bool state;
		auto nodeIngoreState = allowedNodes.find(currentNodeIdx);
		if (nodeIngoreState == allowedNodes.end()) {
			state = true;
			Vector2 nodePos = getNodePosition(currentNodeIdx);
			for (const common::Circle& circle : ignoredAreas) {
				if (circle.contains(nodePos)) {
					state = false;
					break;
				}
			}
			allowedNodes[currentNodeIdx] = state;
		}
		else {
			state = nodeIngoreState->second;
		}

		// Jeœli wêze³ znajduje siê w ignorowanym obszarze, przechodzimy do nastêpnego.
		if (!state) { continue; }

		for (auto connection : _navigationMesh.at(currentNodeIdx).arcs) {
			Segment connectionSegment = Segment(getNodePosition(currentNodeIdx), getNodePosition(connection.first));
			bool isAllowed = true;
			for (const common::Circle& circle : ignoredAreas) {
				if (common::distance(circle.center, connectionSegment) <= circle.radius) {
					isAllowed = false;
					break;
				}
			}
			if (!isAllowed) { continue; }

			int nodeValue = connection.first;
			float costSoFar = current.costSoFar + connection.second;
			float endNodeHeuristic;

			AStarNodeInfo endNodeRecord;
			if (containsNode(closed, nodeValue, endNodeRecord)) {
				if (endNodeRecord.costSoFar <= costSoFar) { continue; }
				removeNode(closed, nodeValue);
				endNodeHeuristic = endNodeRecord.estimatedTotalCost - endNodeRecord.costSoFar;
			}
			else if (containsNode(open, nodeValue, endNodeRecord)) {
				if (endNodeRecord.costSoFar <= costSoFar) { continue; }
				endNodeHeuristic = endNodeRecord.estimatedTotalCost - endNodeRecord.costSoFar;
			}
			else {
				endNodeRecord.nodeValue = nodeValue;
				endNodeHeuristic = estimateDistance(nodeValue, to);
			}

			endNodeRecord.costSoFar = costSoFar;
			endNodeRecord.previous = currentNodeIdx;
			endNodeRecord.estimatedTotalCost = costSoFar + endNodeHeuristic;

			if (!containsNode(open, nodeValue)) {
				open.push_back(endNodeRecord);
				push_heap(open.begin(), open.end(), lowerCost2);
				make_heap(open.begin(), open.end(), lowerCost2);
			}
		}

		closed.push_back(current);
	}
	
	std::vector<int> path;

	if (current.nodeValue == to) {
		do {
			path.push_back(current.nodeValue);
		} while (current.nodeValue != from && containsNode(closed, current.previous, current));
	}

	return path;
}

std::queue<Vector2> GameMap::findPath(const Vector2& from, const Vector2& to, GameDynamicObject* movable) const {
	return findPath(from, to, movable, {});
}

std::queue<Vector2> GameMap::findPath(const Vector2& from, const Vector2& to, 
	GameDynamicObject* movable, const std::vector<common::Circle>& ignoredAreas) const {

	int start = getClosestNavigationNode(from, ignoredAreas);
	int end = isPositionValid(to, ActorRadius) ? getClosestNavigationNode(to, ignoredAreas) : -1;
	if (start == -1 || end == -1) { return std::queue<Vector2>(); }
	else {
		std::vector<int> pathIndices = aStar(start, end, ignoredAreas);
		std::queue<Vector2> result;
		unsigned int size = pathIndices.size();

		if (size > 0) {

			//int first = 0;
			//int last = size - 1;

			//bool keepLooping = true;
			//while (first < last && keepLooping) {
			//	bool condition1 = isMovementValid(Segment(to, getNodePosition(pathIndices.at(first + 1))));
			//	bool condition2 = false;
			//	if (condition1) { ++first; }
			//	if (first < last) {
			//		condition2 = isMovementValid(Segment(from, getNodePosition(pathIndices.at(last - 1))));
			//		if (condition2) { --last; }
			//	}
			//	keepLooping = condition1 || condition2;
			//}

			//if (first == last) {
			//	if (!isMovementValid(Segment(from, to))) {
			//		result.push(getNodePosition(pathIndices.at(first)));
			//	}
			//}
			//else {
			//	for (int i = last; i >= first; --i) {
			//		result.push(getNodePosition(pathIndices.at(i)));
			//	}
			//}

			if (!isMovementValid(movable, to - from)) {
				int first = size - 1;
				if (movable != nullptr && size > 1 && isMovementValid(movable, getNodePosition(pathIndices.at(size - 2)) - movable->getPosition())) {
					--first;
				}
				for (int i = first; i >= 0; --i) {
					result.push(getNodePosition(pathIndices.at(i)));
				}
			}
			result.push(to);
		}
		
		return result;
	}
}

Vector2 GameMap::getNodePosition(int index) const { return _navigationMesh.at(index).position; }

bool GameMap::isMovementValid(GameDynamicObject* movable, const Vector2& movementVector) const {

	float padding = movable->getRadius() + MovementSafetyMargin + common::EPSILON;
	float sqPadding = padding * padding;
	Vector2 pos = movable->getPosition();
	Segment segment = Segment(pos, pos + movementVector);

	for (auto wall : _collisionResolver->broadphaseStatic(segment.from, segment.to, padding)) {
		if (wall->getSqDistanceTo(segment) <= sqPadding) {
			return false;
		}
	}

	for (auto entity : _collisionResolver->broadphaseDynamic(pos, segment.to, padding)) {
		if (movable != entity && entity->isSolid() && common::distance(entity->getPosition(), segment) <= entity->getRadius() + padding) {
			return false;
		}
	}

	return true;
}

bool GameMap::isPositionValid(const Vector2& point, float entityRadius) const {
	float r = entityRadius + common::EPSILON + MovementSafetyMargin;
	for (auto staticObj : _collisionResolver->broadphaseStatic(point, r)) {
		if (staticObj->getDistanceTo(point) <= r) {
			return false;
		}
	}
	return true;
}

GameMap* GameMap::Loader::load(const char* mapFilename) {
	_reader.open(MapsDirectory + mapFilename);

	if (_reader.fail()) {
		throw "Plik '" + String(mapFilename) + "' nie istnieje, jest niedostêpny lub uszkodzony.";
	}

	_map = new GameMap();

	loadMapSize();
	_map->_collisionResolver = new RegularGrid(_map->getWidth(), _map->getHeight(), RegularGridSize);

	loadNavigationPoints();
	loadNavigationMesh();
	_map->_walls = loadStaticObjects();
	
	for (auto staticObj : _map->_walls) {
		_map->_collisionResolver->add(staticObj);
	}

	_reader.close();

	return _map;
}

void GameMap::Loader::loadMapSize() {
	std::string s;
	_reader >> s >> _map->_width >> s >> _map->_height;
}

void GameMap::Loader::loadNavigationPoints() {
	int points, idx;
	float posX, posY;
	std::string s;
	_reader >> s >> points;

	_map->_navigationMesh.reserve(points);

	for (int i = 0; i < points; i++) {
		_reader >> s >> idx >> s >> posX >> s >> posY;
		_map->_navigationMesh.push_back(NavigationNode(posX, posY, idx));
	}
}

void GameMap::Loader::loadNavigationMesh() {
	int connections, fromIdx, toIdx;
	float modifier;
	bool inverse;
	std::string s;
	_reader >> s >> connections;

	for (int i = 0; i < connections; i++) {
		_reader >> s >> fromIdx >> s >> toIdx >> s >> modifier >> s >> inverse;
		NavigationNode* from = &_map->_navigationMesh.at(fromIdx);
		NavigationNode* to = &_map->_navigationMesh.at(toIdx);
		float dist = common::distance(from->position, to->position) * common::abs(modifier);
		from->arcs.push_back(NavigationNode::Arc(toIdx, dist));
		if (inverse) {
			to->arcs.push_back(NavigationNode::Arc(fromIdx, dist));
		}
	}
}

std::vector<GameStaticObject*> GameMap::Loader::loadStaticObjects() {
	float x1, y1, x2, y2, id, p;
	std::string objectType, s;
	std::vector<GameStaticObject*> staticObjects;

	while (_reader >> objectType) {
		if (objectType == "wall:") {
			_reader >> x1 >> y1 >> x2 >> y2 >> s >> id >> s >> p;
			staticObjects.push_back(new Wall(id, Vector2(x1, y1), Vector2(x2, y2), p));
		}
		else {
			throw "Nieprawid³owa struktura pliku mapy! Nie rozpoznano: '" + objectType + "'.";
		}
	}

	return staticObjects;
}

#ifdef _DEBUG

Vector2 GameMap::getClosest(const Vector2& point) const {
	int idx = getClosestNavigationNode(point, {});
	return idx != NULL_IDX ? _navigationMesh[idx].position : Vector2();
}

std::vector<Vector2> GameMap::getNavigationNodes() const {
	std::vector<Vector2> result;
	result.reserve(_navigationMesh.size());
	for (NavigationNode node : _navigationMesh) {
		result.push_back(node.position);
	}
	return result;
}

std::vector<Segment> GameMap::getNavigationArcs() const {
	std::vector<Segment> result;
	for (NavigationNode node : _navigationMesh) {
		Vector2 from = node.position;
		for (auto arc : node.arcs) {
			result.push_back(Segment(from, _navigationMesh.at(arc.first).position));
		}
	}
	return result;
}

//std::vector<Aabb> GameMap::getAabbs() const { return _entities.getAabbs(); }
//
//std::vector<Aabb> GameMap::getRegionsContaining(const common::Circle& circle) {
//	auto regions = _grid.getRegionsContaining(circle.center, circle.radius);
//	std::vector<Aabb> result;
//	result.reserve(regions.size());
//	for (auto region : regions) {
//		result.push_back(Aabb(region->idX * RegularGridSize, region->idY * RegularGridSize, RegularGridSize, RegularGridSize));
//	}
//	return result;
//}

//std::vector<Aabb> GameMap::getRegionsContaining(const Segment& segment) {
//	auto regions = _grid.getRegionsContaining(segment);
//	std::vector<Aabb> result;
//	result.reserve(regions.size());
//	for (auto region : regions) {
//		result.push_back(Aabb(region->idX * RegularGridSize, region->idY * RegularGridSize, RegularGridSize, RegularGridSize));
//	}
//	return result;
//}
//
//std::vector<Aabb> GameMap::getRegionsContaining(const Vector2& from, const Vector2& to, float radius) {
//	auto regions = _grid.getRegionsForMovement(from, to, radius);
//	std::vector<Aabb> result;
//	result.reserve(regions.size());
//	for (auto region : regions) {
//		result.push_back(Aabb(region->idX * RegularGridSize, region->idY * RegularGridSize, RegularGridSize, RegularGridSize));
//	}
//	return result;
//}


/*
void GameMap::Loader::appendIfNotContains(std::vector<GameMap::NavigationNode>& collection, const Vector2& point) const {
	bool isDuplicate = false;
	float epsilon = common::EPSILON;
	for (const NavigationNode& node : collection) {
		if (common::sqDist(point, node.position) <= epsilon) {
			isDuplicate = true;
			break;
		}
	}
	if (!isDuplicate) {		
		collection.push_back(NavigationNode(point.x, point.y, collection.size()));
	}
}

void GameMap::Loader::generateConnections(const String& inputFile, const String& outputFile) {

	_reader.open((MapsDirectory + inputFile).c_str());

	if (_reader.fail()) {
		throw "Plik '" + String(MapsDirectory + inputFile) + "' nie istnieje, jest niedostêpny lub uszkodzony.";
	}

	std::vector<Wall> wallsRaw;
	int width, height;

	String s;
	_reader >> s >> width >> s >> height;

	float x1, y1, x2, y2, id, p;
	String objectType;

	while (_reader >> objectType) {
		if (objectType == "wall:") {
			_reader >> x1 >> y1 >> x2 >> y2 >> s >> id >> s >> p;
			wallsRaw.push_back(Wall(id, Vector2(x1, y1), Vector2(x2, y2), p));
		}
		else {
			throw "Nieprawid³owa struktura pliku mapy! Nie rozpoznano: '" + objectType + "'.";
		}
	}

	_reader.close();
	
	std::vector<Wall> walls;
	walls.reserve(wallsRaw.size());

	std::vector<NavigationNode> points;
	points.reserve(wallsRaw.size() * 4);

	float epsilon = common::EPSILON;
	float k = ActorRadius;// + MovementSafetyMargin;

	for (Wall wall : wallsRaw) {
		auto from = wall.getFrom();
		auto to = wall.getTo();
		if (common::sqDist(from, to) > epsilon) {
			walls.push_back(wall);
			Vector2 v1 = (to - from).normal() * k;
			Vector2 v2 = Vector2(v1.y, -v1.x);

			appendIfNotContains(points, from - v1 + v2);
			appendIfNotContains(points, from - v1 - v2);
			appendIfNotContains(points, to + v1 + v2);
			appendIfNotContains(points, to + v1 - v2);
		}
	}

	GameMap* map = new GameMap();
	map->_walls.initialize(walls);

	// Je¿eli po³¹czenie przechodzi zbyt blisko œciany, to odrzucamy je.
	auto condition1 = [&walls](const Segment& s, float distance) -> bool {
		for (auto elem : walls) {
			if (common::distance(elem.getFrom(), s) < distance
				|| common::distance(elem.getTo(), s) < distance) { 
				return true; 
			} 
		}
		return false;
	};

	// Je¿el punkt nawigacji (inny ni¿ rozwa¿ane po³¹czenie) nale¿y do po³¹czenia, odrzucamy je.
	auto condition2 = [&points](const Segment& s) -> bool {
		float epsilon = common::EPSILON;
		for (NavigationNode node : points) {
			Vector2 point = node.position;
			if (common::sqDist(point, s.from) > epsilon
				&& common::sqDist(point, s.to) > epsilon
				&& common::distance(point, s) < epsilon) {
				return true;
			}
		}
		return false;
	};

	int n = points.size();
	NavigationNode* first;
	NavigationNode* second;
	Vector2 tempVector;

	std::ofstream myfile;
	myfile.open(outputFile);

	myfile << "width: " << width << " height: " << height << "\n\n";

	myfile << "navigation_points: " << n << "\n";
	for (int i = 0; i < n; ++i) {
		myfile << "id: " << points[i].index
			<< " x: " << points[i].position.x
			<< " y: " << points[i].position.y << "\n";
	}

	std::vector<String> connections;
	for (int i = 0; i < n; ++i) {
		first = &points.at(i);
		for (int j = i; j < n; ++j) {
			second = &points.at(j);
			const Segment arc = Segment(first->position, second->position);
			if (first->index < second->index
				&& !map->raycastStatic(arc, tempVector) 
				&& !condition1(arc, ActorRadius)
				&& !condition2(arc)) {
				connections.push_back("from: " + std::to_string(first->index)
					+ " to: " + std::to_string(second->index) + " mul: 1.0 inv: 1\n");
			}
		}
	}

	myfile << "\nconnections: " << connections.size() << "\n";
	for (const String& connection : connections) {
		myfile << connection;
	}

	myfile << "\n";
	for (const Wall& wall : walls) {
		Segment s = wall.getSegment();
		myfile << "wall: " << s.from.x << " " << s.from.y << " "
			<< s.to.x << " " << s.to.y << " id: "
			<< wall.getId() << " priority: " << wall.getPriority() << "\n";
	}

	myfile.close();

	delete map;
}
*/

#endif
