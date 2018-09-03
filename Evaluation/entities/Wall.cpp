#include "Wall.h"

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
