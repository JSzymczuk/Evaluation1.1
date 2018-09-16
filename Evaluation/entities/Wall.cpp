#include "Wall.h"
#include "math/Math.h"

Wall::Wall() : _identifier(-1), _priority(0) {}

Wall::Wall(int identifier, Vector2 from, Vector2 to, int priority)
	: _identifier(identifier), _priority(priority), _from(from), _to(to) {}

Wall::Wall(const Wall& wall) {
	_identifier = wall._identifier;
	_priority = wall._priority;
	_from = wall._from;
	_to = wall._to;
}

Wall::~Wall() {}

int Wall::getId() const { return _identifier; }
int Wall::getPriority() const { return _priority; }
Vector2 Wall::getFrom() const { return _from; }
Vector2 Wall::getTo() const { return _to; }
Segment Wall::getSegment() const { return Segment(_from, _to); }

bool Wall::checkCollision(const Segment& segment) const {
	Vector2 v;
	return common::testSegments(segment, getSegment(), v);
}

float Wall::getSqDistanceTo(const Segment& segment) const {
	return common::sqDist(segment, getSegment());
}

std::vector<Segment> Wall::getBounds() const {
	return { getSegment() };
}

float Wall::getDistanceTo(const Vector2& point) const {
	return common::distance(point, this->getSegment());
}
