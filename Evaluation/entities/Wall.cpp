#include "Wall.h"
#include "math/Math.h"

Wall::Wall(int identifier, Vector2 from, Vector2 to, int priority)
	: StaticEntity(Aabb(from, to)), _identifier(identifier), _priority(priority), _from(from), _to(to) {}

Wall::~Wall() {}

int Wall::getId() const { return _identifier; }
int Wall::getPriority() const { return _priority; }
Vector2 Wall::getFrom() const { return _from; }
Vector2 Wall::getTo() const { return _to; }
Segment Wall::getSegment() const { return Segment(_from, _to); }
size_t Wall::getBoundsSize() const { return 1; }
std::vector<Segment> Wall::getBounds() const {
	return { getSegment() };
}
