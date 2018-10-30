#include "Navigation.h"

#include "entities/Entity.h"
#include "entities/Wall.h"
#include "engine/SegmentTree.h"
#include "main/Configuration.h"
#include "engine/CommonFunctions.h"
#include "entities/Actor.h"
#include "entities/Trigger.h"
#include "engine/TriggerFactory.h"
#include "engine/CollisionResolver.h"
#include "engine/VectorCollisionResolver.h"
#include "engine/TreeCollisionResolver.h"

float GameMap::getWidth() const { return _width; }
float GameMap::getHeight() const { return _height; }

GameMap* GameMap::create(const char* filepath) { return Loader().load(filepath); }

void GameMap::generateConnections(const String& inputFile, const String& outputFile) { 
	Loader().generateConnections(inputFile, outputFile); 
}

void GameMap::destroy(GameMap* map) {
	auto walls = map->_walls;
	for (auto wallPtr : walls) {
		delete wallPtr;
	}
	delete map->_collisionResolver;
	delete map;
}

std::vector<Actor*> GameMap::getActors() const { return _entities; }

std::vector<Trigger*> GameMap::getTriggers() const { return _triggers; }

std::vector<StaticEntity*> GameMap::getWalls() const { return _walls; }

bool GameMap::raycastStatic(const Segment& ray, Vector2& result) const {
	Vector2 rayOrigin = ray.from;
	bool collisionFound = false;
	float minDist;

	for (StaticEntity* staticObj : _collisionResolver->broadphaseStatic(ray.from, ray.to)) {
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

bool GameMap::place(Actor* actor) {
	if (canPlace(actor)) {
		_collisionResolver->add(actor);
		_entities.push_back(actor);
		return true;
	}
	return false;
}

bool GameMap::place(Trigger* trigger) {
	if (canPlace(trigger)) {
		_collisionResolver->add(trigger);
		_triggers.push_back(trigger);
		return true;
	}
	return false;
}

void GameMap::remove(Actor* actor) {
	size_t idx = common::indexOf(_entities, actor);
	size_t n = _entities.size();
	if (idx != n) {
		_collisionResolver->remove(actor);
		common::swapLastAndRemove(_entities, idx);
	}
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

//std::vector<GameDynamicObject*> GameMap::checkCollision(const Vector2& point) { 
//	return _collisionResolver->broadphaseDynamic(point);
//}

//std::vector<GameDynamicObject*> GameMap::checkCollision(const Aabb& area) const { 
//	return _collisionResolver->broadphase(area);
//}

const CollisionResolver* GameMap::getCollisionResolver() const {
	return _collisionResolver;
}

std::vector<DynamicEntity*> GameMap::checkCollision(const Vector2& point, float radius) {
	return _collisionResolver->broadphaseDynamic(point, radius);
}

std::vector<DynamicEntity*> GameMap::checkCollision(const Segment& segment) {
	return _collisionResolver->broadphaseDynamic(segment.from, segment.to);
}

std::vector<Destructible*> GameMap::checkCollisionDestructible(const Vector2& point, float radius) const {
	return getDestructibleInArea(_collisionResolver, point, radius);
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

std::queue<Vector2> GameMap::findPath(const Vector2& from, const Vector2& to, Movable* movable) const {
	return findPath(from, to, movable, {});
}

bool isMovementValid(CollisionResolver* collisionResolver, Movable* movable, const Vector2& movementVector) {
	//float padding = movable->getRadius() + Config.MovementSafetyMargin + common::EPSILON;
	Vector2 pos = movable->getPosition();
	Segment segment = Segment(pos, pos + movementVector);
	return !checkMovementCollisions(collisionResolver, movable, segment);
}

std::queue<Vector2> GameMap::findPath(const Vector2& from, const Vector2& to, 
	Movable* movable, const std::vector<common::Circle>& ignoredAreas) const {

	int start = getClosestNavigationNode(from, ignoredAreas);
	int end = isPositionValid(_collisionResolver, movable, true) ? getClosestNavigationNode(to, ignoredAreas) : -1;
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

			if (!isMovementValid(_collisionResolver, movable, to - from)) {
				int first = size - 1;
				if (size > 1 && isMovementValid(_collisionResolver, movable, getNodePosition(pathIndices.at(size - 2)) - movable->getPosition())) {
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

bool GameMap::canPlace(const DynamicEntity* object) const {
	return isPositionValid(_collisionResolver, object);
}

void GameMap::initializeDynamic(const std::vector<DynamicEntity*>& dynamicObjects) {
	_collisionResolver->initializeDynamic(dynamicObjects);
	for (DynamicEntity* entity : dynamicObjects) {
		// to do (entity nie musi byæ aktorem; ta funkcja powinna byæ prywatna)
		_entities.push_back(dynamic_cast<Actor*>(entity));
	}
}

GameMap* GameMap::Loader::load(const char* mapFilename) {
	_reader.open(mapFilename);

	if (_reader.fail()) {
		throw "Plik '" + String(mapFilename) + "' nie istnieje, jest niedostêpny lub uszkodzony.";
	}

	_map = new GameMap();

	loadMapSize();
	
	if (Config.CollisionResolver == "AabbTree") {
		_map->_collisionResolver = new TreeCollisionResolver();
	}
	else if (Config.CollisionResolver == "RegularGrid") {
		_map->_collisionResolver = new RegularGrid(_map->getWidth(), _map->getHeight(), Config.RegularGridSize);
	}
	else {
		_map->_collisionResolver = new VectorCollisionResolver();
	}

	loadNavigationPoints();
	loadNavigationMesh();
	_map->_walls = loadStaticObjects();
	
	for (auto staticObj : _map->_walls) {
		_map->_collisionResolver->add(staticObj);
	}

	for (auto dynamicObj : loadTriggers()) {
		if (!_map->place(dynamicObj)) {
			delete dynamicObj;
		}
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

std::vector<StaticEntity*> GameMap::Loader::loadStaticObjects() {
	size_t staticObjectsSize;
	float x1, y1, x2, y2, id, p;
	std::string objectType, s;
	std::vector<StaticEntity*> staticObjects;
	_reader >> s >> staticObjectsSize;
	staticObjects.reserve(staticObjectsSize);

	for (size_t i = 0; i < staticObjectsSize; i++) {
		_reader >> objectType;
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

std::vector<Trigger*> GameMap::Loader::loadTriggers() {
	size_t dynamicObjectsSize;
	float x, y;
	std::string objectType, s;
	std::vector<Trigger*> dynamicObjects;
	_reader >> s >> dynamicObjectsSize;
	dynamicObjects.reserve(dynamicObjectsSize);

	for (size_t i = 0; i < dynamicObjectsSize; ++i) {
		_reader >> objectType >> x >> y;
		if (objectType == Config.MedPackName) {
			dynamicObjects.push_back(TriggerFactory::create(TriggerType::HEALTH, Vector2(x, y)));
		}
		else if (objectType == Config.ArmorPackName) {
			dynamicObjects.push_back(TriggerFactory::create(TriggerType::ARMOR, Vector2(x, y)));
		}
		else {
			dynamicObjects.push_back(TriggerFactory::create(objectType, Vector2(x, y)));
		}
	}

	return dynamicObjects;
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


void GameMap::Loader::generateConnections(const String& inputFile, const String& outputFile) {

	_reader.open(inputFile);

	if (_reader.fail()) {
		throw "Plik '" + String(inputFile) + "' nie istnieje, jest niedostêpny lub uszkodzony.";
	}

	int width, height, size;
	std::vector<NavigationNode> points;
	std::vector<Wall> walls;

	float epsilon = common::EPSILON;

	String s;
	_reader >> s >> width >> s >> height;

	int idx;
	float posX, posY;
	_reader >> s >> size;

	points.reserve(size);

	for (int i = 0; i < size; i++) {
		_reader >> s >> idx >> s >> posX >> s >> posY;
		points.push_back(NavigationNode(posX, posY, idx));
	}

	float x1, y1, x2, y2, id, p;
	String objectType;
	
	_reader >> s >> size;
	walls.reserve(size);

	for (int i = 0; i < size; i++) {
		_reader >> objectType;
		if (objectType == "wall:") {
			_reader >> x1 >> y1 >> x2 >> y2 >> s >> id >> s >> p;
			Vector2 from(x1, y1), to(x2, y2);
			if (common::sqDist(from, to) > epsilon) {
				walls.push_back(Wall(id, Vector2(x1, y1), Vector2(x2, y2), p));
			}
		}
		else {
			throw "Nieprawid³owa struktura pliku mapy! Nie rozpoznano: '" + objectType + "'.";
		}
	}

	_reader.close();
	
	float k = Config.ActorRadius;
	
	// Je¿eli po³¹czenie przechodzi zbyt blisko œciany, to odrzucamy je.
	auto condition1 = [&walls](const Segment& s, float distance) -> bool {
		distance *= distance;
		for (auto elem : walls) {
			if (common::sqDist(s, elem.getSegment()) < distance) { 
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
				&& !condition1(arc, k)
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
}

#endif
