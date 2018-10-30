#include "Movable.h"
#include "entities/Trigger.h"
#include "entities/Actor.h"
#include "engine/CollisionResolver.h"
#include "main/Game.h"
#include "entities/Wall.h"
#include "engine/CommonFunctions.h"


Vector2 Movable::getPosition() const { return DynamicEntity::getPosition(); }

Movable::~Movable() {}

bool Movable::isWaiting() const { return _isWaiting; }

bool Movable::isSpotting() const { return Spotter::isSpotting(); }

CollisionResolver* Movable::getCollisionResolver() const { return DynamicEntity::getCollisionResolver(); }

float Movable::getRotation() const { return _rotation; }

Movable::Movable(const Vector2& position)
	: DynamicEntity(position, common::PI_F / 2) {
	_velocity = Vector2();
	_rotation = 0;
	_isRotating = false;
	_isWaiting = false;
	_recalculations = 0;
	_positionHistoryLength = 0;
	_nextHistoryIdx = 0;

	for (size_t i = 0; i < Config.ActionPositionHistoryLength; ++i) {
		_positionHistory.push_back(_position);
	}
}

std::queue<Vector2> Movable::getCurrentPath() const { return _path; }

void Movable::setPreferredVelocity(const Vector2& velocity) { _preferredVelocity = velocity; }

Vector2 Movable::getPreferredVelocity() const { return _preferredVelocity; }

Vector2 Movable::getVelocity() const { return _velocity; }


Vector2 Movable::getShortGoal() const { return _nextSafeGoal; }

Vector2 Movable::getLongGoal() const { return _path.empty() ? _position : _path.back(); }

float Movable::estimateRemainingDistance() const {
	auto path = this->getCurrentPath();
	float distance = 0;
	if (!path.empty()) {
		Vector2 last = path.front();
		path.pop();
		Vector2 next;
		while (!path.empty()) {
			next = path.front();
			path.pop();
			distance += common::distance(last, next);
			next = last;
		}
	}
	return distance;
}

bool Movable::hasPositionChanged() const { return _velocity.lengthSquared() > common::EPSILON; }

float Movable::calculateRotation() const {
	bool clockwise = common::isAngleBetween(_desiredOrientation, _orientation, _orientation + common::PI_F);
	float diff = clockwise ? common::measureAngle(_orientation, _desiredOrientation)
		: common::measureAngle(_desiredOrientation, _orientation);
	if (diff > common::PI_F) { return 0; }
	if (diff < Config.ActorRotationSpeed) { return (clockwise ? diff : -diff) / Config.ActorRotationSpeed; }
	return clockwise ? 1.0f : -1.0f;
}

MovementCheckResult Movable::checkMovement() const {
	Vector2 futurePosition = _position + _velocity;
	float r = getRadius();
	auto potentialColliders = getDynamicObjectsInArea(getCollisionResolver(), futurePosition, r);

	MovementCheckResult result;
	result.allowed = true;

	common::Circle futureCollisionArea = { futurePosition, r };

	for (DynamicEntity* t : potentialColliders) {
		if (t != this && common::testCircles(futureCollisionArea, { t->getPosition(), t->getRadius() })) {
			if (t->isSolid()) { result.allowed = false; }
			CollisionResponder* resp = dynamic_cast<CollisionResponder*>(t);
			if (resp != nullptr) {
				result.responders.push_back(resp);
			}
		}
	}

	if (checkMovementCollisions(getCollisionResolver(), this, Segment(_position, futurePosition))) {
		//_path.size() > 0 ? ActorRadius - MovementSafetyMargin : ActorRadius)) {
		result.allowed = false;
		//Logger::log("Actor " + _name + " movement wasn't allowed.");
	}

	return result;
}

std::vector<Candidate> Movable::computeCandidates(const std::vector<VelocityObstacle>& vo) const {

	std::vector<Candidate> result;
	Vector2 pos = _position;
	size_t n = vo.size();

	// Czy preferowany wektor ruchu jest dopuszczalny?
	bool accept = true;
	for (size_t i = 0; i < n && accept; ++i) {
		Vector2 vec = pos + _preferredVelocity - vo[i].apex;
		if (common::cross(vo[i].side1, vec) >= 0 && common::cross(vec, vo[i].side2) >= 0) {
			accept = false;
		}
	}

	float speed = getMaxSpeed();
	float voCheckRadius = Config.ActorVOCheckRadius;

	if (accept && _preferredVelocity.lengthSquared() > common::EPSILON) {
		Candidate c;
		c.velocity = _preferredVelocity;
		c.difference = 0;
		c.collisionFreeDistance = minDistanceWithoutCollision(_preferredVelocity, voCheckRadius);
		if (c.collisionFreeDistance > speed) {
			result.push_back(c);
		}
	}

	Vector2 currentVelocity = _velocity;

	// SprawdŸ wektory ruchu wzd³u¿ krawêdzi VO.
	for (size_t i = 0; i < n; ++i) {
		Vector2 v1 = common::adjustLength(vo[i].side1, speed), v2 = common::adjustLength(vo[i].side2, speed);
		Vector2 point1 = vo[i].apex + v1, point2 = vo[i].apex + v2;
		bool accept1 = true, accept2 = true;
		for (size_t j = 0; j < n && (accept1 || accept2); ++j) {
			if (i == j) { continue; }
			if (accept1) {
				Vector2 vec1 = point1 - vo[j].apex;
				if (common::cross(vo[j].side1, vec1) > 0 && common::cross(vec1, vo[j].side2) > 0) {
					accept1 = false;
				}
			}
			if (accept2) {
				Vector2 vec2 = point2 - vo[j].apex;
				if (common::cross(vo[j].side1, vec2) > 0 && common::cross(vec2, vo[j].side2) > 0) {
					accept2 = false;
				}
			}
		}
		if (accept1) {
			Candidate c;
			c.velocity = v1;
			c.difference = common::distance(v1, _preferredVelocity);
			c.collisionFreeDistance = minDistanceWithoutCollision(v1, voCheckRadius);
			result.push_back(c);
		}
		if (accept2) {
			Candidate c;
			c.velocity = v2;
			c.difference = common::distance(v2, _preferredVelocity);
			c.collisionFreeDistance = minDistanceWithoutCollision(v2, voCheckRadius);
			result.push_back(c);
		}
	}

	return result;
}

float Movable::minDistanceWithoutCollision(const Vector2& direction, float maxDistance) const {
	Vector2 dirVec = common::adjustLength(direction, maxDistance);
	Vector2 endPoint = _position + dirVec;
	Segment segment(_position, endPoint);
	float minDist = maxDistance;
	Segment movementSegment = Segment(_position, endPoint);
	float r = getRadius();

	auto broadphaseResultDynamic = getDynamicObjectsOnLine(getCollisionResolver(), segment);

	for (auto c : broadphaseResultDynamic) {
		Vector2 otherPos = c->getPosition();
		float otherRadius = c->getRadius();
		if (c != this && c->isSolid() && common::distance(otherPos, movementSegment) <= r + otherRadius + common::EPSILON) {
			float dist = (otherPos - _position).length() - otherRadius - r;
			if (dist < minDist) { minDist = dist; }
		}
	}

	auto broadphaseResultStatic = getStaticObjectsOnLine(getCollisionResolver(), segment);

	for (auto elem : broadphaseResultStatic) {
		float dist = getDistanceTo(elem, _position) - r;
		if (dist < minDist) { minDist = dist; }
	}

	return minDist;
}

std::pair<Vector2, Vector2> Movable::getVOSides(const Vector2& point, const common::Circle& circle) const {
	Vector2 c = circle.center;
	Vector2 pc = c - point;
	float r = circle.radius;
	float d = pc.length();
	float d2 = d * d;
	float r2 = r * r;
	float d1 = (d2 - r2) / d;
	Vector2 q = point + pc * (d1 / d);
	Vector2 v = Vector2(c.y - point.y, point.x - c.x) * (sqrtf(d2 - r2 - d1 * d1) / d);
	return std::make_pair((q + v - point).normal(), (q - v - point).normal());
}

std::vector<VelocityObstacle> Movable::getVelocityObstacles(const std::vector<Spottable*>& obstacles) const {
	std::vector<VelocityObstacle> result;
	result.reserve(obstacles.size());
	Vector2 pos = _position;
	float r = getRadius() + common::EPSILON;
	for (Spottable* other : obstacles) {
		if (other->isSolid()) {
			//Vector2D apex = cder->is_static() ? pos : pos + (cder->predicted_velocity() + movable->velocity) / 2;
			Vector2 apex = pos;
			common::Circle voCircle = common::Circle(other->getPosition(), r + other->getRadius());
			if (!voCircle.contains(pos)) {
				auto sides = getVOSides(pos, voCircle);
				result.push_back({ apex, sides.first, sides.second, other });
			}
		}
	}
	return result;
}

void Movable::lookAt(const Vector2& point) {
	_desiredOrientation = common::angleFromTo(_position, point);
	_isRotating = true;
}

std::vector<Segment> Movable::getSegmentsNearGoal() const {
	std::vector<Segment> result;
	if (!_path.empty()) {
		float dist = (1.1f * getRadius() + common::EPSILON) * common::SQRT_2_F;
		Vector2 point = _path.front();

		auto broadphaseResult = getCollisionResolver()->broadphaseStatic(point, dist);
		
		for (StaticEntity* staticObj : broadphaseResult) {
			for (const Segment& seg : staticObj->getBounds()) {
				if (common::distance(point, seg) <= dist) {
					result.push_back(seg);
				}
			}
		}
	}
	return result;
}

Vector2 Movable::getNextSafeGoal() const {
	if (!_path.empty()) {
		auto walls = getSegmentsNearGoal();
		if (walls.size() == 2) {
			Vector2 p11 = walls.at(0).from,
				p12 = walls.at(0).to,
				p21 = walls.at(1).from,
				p22 = walls.at(1).to;

			bool validFlag = false;
			Vector2 commonPoint, otherPoint1, otherPoint2;
			if (common::sqDist(p11, p21) < common::EPSILON) {
				validFlag = true;
				commonPoint = p11;
				otherPoint1 = p12;
				otherPoint2 = p22;
			}
			else if (common::sqDist(p11, p22) < common::EPSILON) {
				validFlag = true;
				commonPoint = p11;
				otherPoint1 = p12;
				otherPoint2 = p21;
			}
			if (common::sqDist(p12, p21) < common::EPSILON) {
				validFlag = true;
				commonPoint = p12;
				otherPoint1 = p11;
				otherPoint2 = p22;
			}
			else if (common::sqDist(p12, p22) < common::EPSILON) {
				validFlag = true;
				commonPoint = p12;
				otherPoint1 = p11;
				otherPoint2 = p21;
			}

			if (validFlag) {
				Vector2 pos = _position;
				bool o1 = common::triangleOrientation(otherPoint1, commonPoint, pos)
					== common::triangleOrientation(otherPoint1, commonPoint, otherPoint2);
				bool o2 = common::triangleOrientation(otherPoint2, commonPoint, pos)
					== common::triangleOrientation(otherPoint2, commonPoint, otherPoint1);

				Vector2 w1 = (p12 - p11).normal();
				Vector2 w2 = (p22 - p21).normal();

				float temp = w1.x; w1.x = w1.y; w1.y = temp;
				temp = w2.x; w2.x = w2.y; w2.y = temp;

				Vector2 q11 = commonPoint + w1, q12 = commonPoint - w1;
				Vector2 q21 = commonPoint + w2, q22 = commonPoint - w2;

				Vector2 translation;

				if (o1 == o2) {
					translation = (common::sqDist(pos, q11) < common::sqDist(pos, q12) ? w1 : -w1)
						+ (common::sqDist(pos, q21) < common::sqDist(pos, q22) ? w2 : -w2);
				}
				else {
					if (o2) {
						translation = (common::sqDist(pos, q11) < common::sqDist(pos, q12) ? w1 : -w1)
							+ (common::sqDist(pos, q21) < common::sqDist(pos, q22) ? -w2 : w2);
					}
					else {
						translation = (common::sqDist(pos, q11) < common::sqDist(pos, q12) ? -w1 : w1)
							+ (common::sqDist(pos, q21) < common::sqDist(pos, q22) ? w2 : -w2);
					}
				}

				return commonPoint + translation.normal() * common::SQRT_2_F
					* (Config.ActorRadius + Config.MovementSafetyMargin + common::EPSILON);
			}
		}
		return _path.front();
	}
	return _position;
}

float Movable::getDistanceToGoal() const {
	if (_path.empty()) { return 0; }
	return (_position - _nextSafeGoal).length();
}

void Movable::move(const std::queue<Vector2>& path) {
	//Logger::log("Actor " + _name + " chose new destination.");
	if (!path.empty()) {
		_path = path;
		_lastDestination = path.empty() ? _position : path.back();
		_nextSafeGoal = getNextSafeGoal();
	}
	else {
		abortMovement(/*"Actor " + _name + " can't reach this destination.", */true);
	}
}

void Movable::abortMovement(/*String loggerMessage, */bool resetCounter) {
	_path = {};
	_preferredVelocity = Vector2();
	_velocity = Vector2();
	_isWaiting = false;
	_nextSafeGoal = _position;
	_lastDestination = _position;
	_positionHistoryLength = 0;
	_nextHistoryIdx = 0;
	if (resetCounter) {
		_recalculations = 0;
	}
	clearPositionHistory();
	//Logger::log(loggerMessage);
}

void Movable::stop() {
	abortMovement(/*"Actor " + _name + " stopped.", */true);
}

void Movable::update(GameTime time) {
	updateMovement(time);
	updateOrientation(time);

	CollisionResolver* collisionResolver = getCollisionResolver();
	if (collisionResolver != nullptr && hasPositionChanged()) {
		collisionResolver->update(this);
	}
}

bool Movable::isMoving() const { return _path.size() > 0 || _preferredVelocity.lengthSquared() > common::EPSILON; }

bool Movable::isStrayingFromPath() const { return !_isStrictlyFollowingPath; }

bool Movable::isRotating() const { return _isRotating; }

void Movable::setPreferredVelocityAndSafeGoal() {
	if (!_path.empty()) {
		Vector2 goal = _path.back();
		if (!checkMovementCollisions(getCollisionResolver(), this, Segment(_position, goal))) {
			if (common::sqDist(_position, goal) < common::sqr(Config.MovementGoalMargin)) {
				abortMovement(/*"Actor " + _name + " reached its destination.", */true);
			}
			else {
				_preferredVelocity = goal - _position;
				_isStrictlyFollowingPath = false;
			}
		}
		else {
			if ((_nextSafeGoal - _position).lengthSquared() < common::sqr(Config.MovementGoalMargin)) {
				_path.pop();
				_nextSafeGoal = getNextSafeGoal();
			}
			if (!_path.empty()) {
				_preferredVelocity = _nextSafeGoal - _position;
				_isStrictlyFollowingPath = true;
			}
			else {
				abortMovement(/*"Actor " + _name + " reached its destination.", */true);
			}
		}
	}
}

Vector2 Movable::selectVelocity(const std::vector<Candidate>& candidates) const {
	if (candidates.empty()) { return Vector2(); }
	size_t min = 0;
	auto fval = [](Candidate c) -> float {
		return c.difference + (Config.ActorVOCheckRadius - c.collisionFreeDistance) / Config.ActorVOCheckRadius;
	};
	float minval = fval(candidates[min]);
	size_t n = candidates.size();
	for (size_t i = 1; i < n; ++i) {
		if (fval(candidates[i]) < minval) {
			min = i;
		}
	}
	return candidates[min].velocity;
}

std::vector<Spottable*> Movable::getObjectsInViewAngle() const {
	float angle = common::radians(Config.ActorVOCheckAngle);
	float from = _orientation - angle;
	float to = _orientation + angle;

	float r = getRadius();
	float safetyMargin = Config.MovementSafetyMargin;
	float voCheckRadius = Config.ActorVOCheckRadius;

	Segment fromSeg = Segment(_position, _position + Vector2(cosf(from), sinf(from)) * voCheckRadius);
	Segment toSeg = Segment(_position, _position + Vector2(cosf(to), sinf(to)) * voCheckRadius);
	//voCheckRadius *= voCheckRadius;
	float temp = common::sqr(2 * r + common::EPSILON + safetyMargin + getMaxSpeed());

	std::vector<Spottable*> result;
	for (Spottable* other : getSpottedObjects()) {
		Vector2 otherPos = other->getPosition();
		if (
			(common::isAngleBetween(common::angleFromTo(_position, otherPos), from, to)
				|| common::distance(otherPos, fromSeg) <= r || common::distance(otherPos, toSeg) <= r)
			&& 
			common::distance(otherPos, _position) - other->getRadius() <= voCheckRadius
			|| common::sqDist(otherPos, _position) <= temp
			) {
			result.push_back(other);
		}
	}
	return result;
}

void Movable::updateMovement(GameTime time) {
	Spotter::update(time);
	if (isMoving()) {
		setPreferredVelocityAndSafeGoal();
		_velocity = selectVelocity(computeCandidates(getVelocityObstacles(getObjectsInViewAngle())));

		if (_velocity.lengthSquared() > common::EPSILON) {
			_velocity = _velocity.normal() * getMaxSpeed();
		}

		MovementCheckResult movementCheckResult = checkMovement();

		bool oscilationDetected = isOscilating();
		if (movementCheckResult.allowed && !oscilationDetected) {
			_position += _velocity;
			saveCurrentPositionInHistory();
			_isWaiting = false;

			if (isLookingStraight() && _velocity.lengthSquared() > common::EPSILON) {
				_desiredOrientation = common::angle(_velocity);
				_isRotating = true;
			}

			CollisionInvoker::invokeCollision(movementCheckResult.responders, time);
		}
		else if (_isWaiting || oscilationDetected) {
			if (time - _waitingStarted > (_recalculations > 0 ? Config.MaxRecalculatedWaitingTime : Config.MaxMovementWaitingTime)) {
				if (!_path.empty()) {
					Vector2 destination = _path.back();
					abortMovement(false);
					if (_recalculations < Config.MaxRecalculations) {
						++_recalculations;
						move(Game::getInstance()->getMap()->findPath(_position, destination, this, {
							common::Circle(_position, 50 * (_recalculations + 1))
							}));
					}
				}
				else {
					stop();
				}
			}
		}
		else {
			_isWaiting = true;
			_waitingStarted = time;
			_velocity.x = 0;
			_velocity.y = 0;
		}
	}
	else {
		std::vector<CollisionResponder*> responders;
		float r = getRadius();
		auto potentialColliders = getDynamicObjectsInArea(getCollisionResolver(), _position, r);
		common::Circle selfCircle = { _position, r };
		for (DynamicEntity* t : potentialColliders) {
			if (t != this && common::testCircles(selfCircle, { t->getPosition(), t->getRadius() })) {
				CollisionResponder* resp = dynamic_cast<CollisionResponder*>(t);
				if (resp != nullptr) {
					responders.push_back(resp);
				}
			}
		}
		CollisionInvoker::invokeCollision(responders, time);
	}
}

void Movable::saveCurrentPositionInHistory() {
	++_nextHistoryIdx;
	if (_nextHistoryIdx == Config.ActionPositionHistoryLength) {
		_nextHistoryIdx = 0;
	}
	if (_positionHistoryLength < Config.ActionPositionHistoryLength) {
		++_positionHistoryLength;
	}
	_positionHistory[_nextHistoryIdx].x = _position.x;
	_positionHistory[_nextHistoryIdx].y = _position.y;
}

bool Movable::isOscilating() const {
	if (_positionHistoryLength == Config.ActionPositionHistoryLength) {
		Vector2 pos = _positionHistory[0];
		float margin = common::sqr(Config.ActorOscilationRadius);
		for (int i = 1; i < _positionHistoryLength; ++i) {
			if (common::sqDist(pos, _positionHistory[i]) > margin) {
				return false;
			}
		}
		//Logger::log("Oscilation detected!");
		return true;
	}
	return false;
}

void Movable::clearPositionHistory() {
	float x = _position.x;
	float y = _position.y;
	size_t n = Config.ActionPositionHistoryLength;
	for (int i = 0; i < n; ++i) {
		_positionHistory[i].x = x;
		_positionHistory[i].y = y;
	}
}

bool Movable::updateOrientation(GameTime time) {
	if (isRotating()) {
		_orientation = common::normalizeAngle(_orientation);
		_rotation = calculateRotation();
		_orientation += _rotation * Config.ActorRotationSpeed;
		if (common::abs(_orientation - _desiredOrientation) < common::EPSILON) {
			_isRotating = false;
		}
		return true;
	}
	return false;
}

void Movable::onCollision(CollisionInvoker* invoker, GameTime time) {

}

bool Movable::isStaticElement() const { return false; }