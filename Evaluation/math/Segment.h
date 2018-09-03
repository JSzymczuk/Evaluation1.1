#pragma once

#include "Aabb.h"

class Segment {
public:
	Vector2 from;
	Vector2 to;

	Segment() : Segment(Vector2(), Vector2()) {}

	Segment(Vector2 from, Vector2 to) : from(from), to(to) { }

	Segment(const Segment& segment) : Segment(segment.from, segment.to) {}

	~Segment() {}

	void transposeX(int x) {
		from.x += x;
		to.x += x;
	}

	void transposeY(int y) {
		from.y += y;
		to.y += y;
	}

	void transpose(Vector2 v) {
		from += v;
		to += v;
	}

	Aabb getAabb() {
		return Aabb(from, to);
	}

	float sqLength() const {
		float dx = to.x - from.x;
		float dy = to.y - from.y;
		return dx * dx + dy * dy;
	}
};

