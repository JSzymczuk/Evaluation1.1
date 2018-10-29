#pragma once

#include <vector>
#include "math/Segment.h"
#include "entities/Entity.h"

class Wall : public StaticEntity {
public:
	int getId() const;
	int getPriority() const;
	Vector2 getFrom() const;
	Vector2 getTo() const;
	Segment getSegment() const;
	std::vector<Segment> getBounds() const override;
	size_t getBoundsSize() const override;
	
	Wall(int identifier, Vector2 from, Vector2 to, int priority);
	~Wall();

private:
	int _identifier;
	int _priority;
	Vector2 _from;
	Vector2 _to;
};
