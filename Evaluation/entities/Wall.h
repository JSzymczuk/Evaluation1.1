#pragma once

#include "math/Segment.h"

class Wall {
public:
	int getId() const;
	int getPriority() const;
	Vector2 getFrom() const;
	Vector2 getTo() const;
	Segment getSegment() const;
	bool checkCollision(const Segment& segment) const;
	float getSqDistanceTo(const Segment& segment) const;

	Wall();
	Wall(int identifier, Vector2 from, Vector2 to, int priority);
	Wall(const Wall& wall);
	~Wall();

private:
	int _identifier;
	int _priority;
	Vector2 _from;
	Vector2 _to;
};
