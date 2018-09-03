#include "Math.h"

namespace common {
	
	// Zwraca wartoœæ pomiêdzy end1 oraz end2 najbli¿sz¹ from, ale nie dalsz¹ ni¿ to.
	bool getClosestValue(float from, float to, float end1, float end2, float& result) {
		if (to > from) {
			if (end2 < end1) swap(end1, end2);
			if (end2 < from || end1 > to) return false;
			if (end1 <= from) {
				result = from;
				return true;
			}
			else {
				result = end1;
				return true;
			}
		}
		else {
			if (end1 < end2) swap(end1, end2);
			if (end2 > from || end1 < to) return false;
			if (end1 >= from) {
				result = from;
				return true;
			}
			else {
				result = end1;
				return true;
			}
		}
	}

	// Zwraca wspó³rzêdne punktu przeciêcia równoleg³ych odcinków sparametryzowanych:
	// p1 + t * (q1 - p1) oraz p2 + u * (q2 - p2), gdzie t, u in [0; 1].
	bool testCollinearSegments(const Vector2& p1, const Vector2& q1, const Vector2& p2, const Vector2& q2, Vector2& result) {
		if (abs(q1.x - p1.x) < EPSILON) {
			float y;
			if (getClosestValue(p1.y, q1.y, p2.y, q2.y, y)) {
				result = Vector2((q1.x + p1.x) / 2, y);
				return true;
			}
			return false;
		}
		else {
			float x;
			if (getClosestValue(p1.x, q1.x, p2.x, q2.x, x)) {
				float t = (x - p1.x) / (q1.x - p1.x);
				result = Vector2(x, p1.y * (1 - t) + q1.y * t);
				return true;
			}
			return false;
		}
	}

	Vector2 getClosestPoint(const Vector2& from, const Vector2& p1, const Vector2& p2) {
		return sqDist(from, p1) < sqDist(from, p2) ? p1 : p2;
	}

	bool pointsCollinear(const Vector2& p1, const Vector2& p2, const Vector2& p3) {
		return abs(p1.x * p2.y + p2.x * p3.y + p3.x * p1.y
			- p1.y * p2.x - p2.y * p3.x - p3.y * p1.x) < EPSILON;
	}

	bool testSegments(const Vector2& p1, const Vector2& q1, const Vector2& p2, const Vector2& q2, Vector2& result) {
		Vector2 r = q1 - p1;
		Vector2 s = q2 - p2;
		float rs = cross(r, s);
		if (abs(rs) < EPSILON) {
			if (pointsCollinear(p1, p2, q1)) {
				return testCollinearSegments(p1, q1, p2, q2, result);
			}
			return false;
		}
		float t = cross(p2 - p1, s) / rs;
		float u = cross(p2 - p1, r) / rs;
		if (0.0f <= t && t <= 1.0f && 0.0f <= u && u <= 1.0f) {
			result = p1 + r * t;
			return true;
		}
		return false;
	}

	bool testSegments(const Segment& segment1, const Segment& segment2, Vector2& result) {
		return testSegments(segment1.from, segment1.to, segment2.from, segment2.to, result);
	}

	// Zwraca punkt przeciêcia prostych: o1 + d1 * t oraz o2 + d2 * u. 
	Vector2 testLines(const Vector2& o1, const Vector2& d1, const Vector2& o2, const Vector2& d2) {
		return o1 + d1 * cross(o2 - o1, d2) / cross(d1, d2);
	}

	Vector2 truncate(const Vector2& vec, float length) {
		float sqLth = vec.lengthSquared();
		if (sqLth <= sqr(length)) { return vec; }
		else { return vec * (length / sqrtf(sqLth)); }
	}

	Segment extendSegment(const Segment& segment, const Aabb& box) {
		Vector2 origin = segment.from;
		Vector2 direction = segment.to - origin;

		if (!box.contains(origin)) { return segment; }

		Vector2 topLeft = box.getTopLeft();
		float width = box.getWidth();
		float height = box.getHeight();
		Vector2 bottomRight = topLeft + Vector2(width, height);

		Vector2 points[4];

		bool isHorizontal = abs(direction.y) < EPSILON;
		bool isVertical = abs(direction.x) < EPSILON;

		if (isHorizontal) {
			// odicnek jest poziomy
			return Segment(Vector2(topLeft.x, origin.y), Vector2(bottomRight.x, origin.y));
		}
		if (isVertical) {
			// odicnek jest pionowy
			return Segment(Vector2(origin.x, topLeft.y), Vector2(origin.x, bottomRight.y));
		}

		points[0] = testLines(origin, direction, topLeft, Vector2(width, 0));
		points[1] = testLines(origin, direction, topLeft, Vector2(0, height));
		points[2] = testLines(origin, direction, bottomRight, Vector2(-width, 0));
		points[3] = testLines(origin, direction, bottomRight, Vector2(0, -height));

		if (sign(direction.x) == sign(direction.y)) {
			// Jeœli odcinek skierowany: \ 
			return Segment(getClosestPoint(origin, points[0], points[1]), getClosestPoint(origin, points[2], points[3]));
		}
		else {
			// Jeœli odcinek sierowany: /
			return Segment(getClosestPoint(origin, points[0], points[3]), getClosestPoint(origin, points[1], points[2]));
		}
	}

	float distance(const Vector2& point, const Segment& segment) {
		Vector2 from = segment.from;
		Vector2 to = segment.to;
		float lth = segment.sqLength();

		if (lth < common::EPSILON) return distance(point, from);

		float t = common::max(0, common::min(1, (point - from).dot(to - from) / lth));
		Vector2 proj = from + (to - from) * t;

		return distance(point, proj);
	}

	bool testCircles(const Circle& circle1, const Circle& circle2) {
		return (circle1.center - circle2.center).lengthSquared() <= sqr(circle1.radius + circle2.radius);
	}

	float measureAngle(float from, float to) {
		return normalizeAngle(to - from);
	}

	bool isAngleBetween(float angle, float angle1, float angle2) {
		float a1 = measureAngle(angle1, angle);
		float a2 = measureAngle(angle1, angle2);
		if (a1 < 0) { a1 += 2 * PI_F; }
		if (a2 < 0) { a2 += 2 * PI_F; }
		return a1 <= a2;
	}
	
	bool testCircleAndSegment(const Circle& circle, const Segment& segment) {
		if (circle.contains(segment.from) || circle.contains(segment.to)) { return true; }

		Vector2 v = segment.to - segment.from;
		Vector2 m = segment.from - circle.center;

		float a = v.lengthSquared();
		float b = 2 * (v.x * m.x + v.y * m.y);
		float c = sqr(m.x) + sqr(m.y) - sqr(circle.radius);

		float discr = b * b - 4 * a * c;
		if (discr < 0) return false;

		float d = sqrtf(discr), temp = 1 / (2 * a);
		float t1 = (-b - d) * temp;
		float t2 = (-b + d) * temp;

		if (t1 >= 0 && t1 <= 1 || t2 >= 0 && t2 <= 1) { return true; }
		return false;
	}

	Vector2 rotatePoint(const Vector2& point, const Vector2& origin, float angle) {
		return rotatePoint(point, origin, cosf(angle), sinf(angle));
	}

	Vector2 rotatePoint(const Vector2& point, const Vector2& origin, float cosAngle, float sinAngle) {
		float dx = point.x - origin.x;
		float dy = point.y - origin.y;
		return Vector2(cosAngle * dx - sinAngle * dy + origin.x, sinAngle * dx + cosAngle * dy + origin.y);
	}

	float sqDist(const Segment& s1, const Segment& s2) {
		Vector2 v1, v2;
		return sqDist(s1, s2, v1, v2);
	}

	float sqDist(const Segment& s1, const Segment& s2, Vector2& point1, Vector2& point2) {

		Vector2 d1 = s1.to - s1.from; // wektor kierunkowy s1
		Vector2 d2 = s2.to - s2.from; // wektor kierunkowy s2
		Vector2 r = s1.from - s2.from;
		float a = d1.dot(d1);
		float e = d2.dot(d2);
		float f = d2.dot(r);

		float s, t;

		if (a <= common::EPSILON && e <= common::EPSILON) {
			// obydwa odcinki degeneruj¹ siê do punktu
			point1 = s1.from;
			point2 = s2.from;
			return sqDist(point1, point2);
		}
		if (a <= common::EPSILON) {
			// pierwszy odcinek degeneruje siê do punktu
			s = 0.0f;
			t = f / e; // s = 0 => t = (b*s + f) / e = f / e
			t = clamp(t, 0.0f, 1.0f);
		}
		else {
			float c = d1.dot(r);
			if (e <= common::EPSILON) {
				// drugi odcinek degeneruje siê do punktu
				t = 0.0f;
				s = clamp(-c / a, 0.0f, 1.0f); // t = 0 => s = (b*t - c) / a = -c / a
			}
			else {
				float b = d1.dot(d2);
				float denom = a * e - b * b;
				if (denom != 0.0f) {
					s = clamp((b*f - c * e) / denom, 0.0f, 1.0f);
				}
				else s = 0.0f;
				t = (b*s + f) / e;
				if (t < 0.0f) {
					t = 0.0f;
					s = clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f) {
					t = 1.0f;
					s = clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}
		point1 = s1.from + d1 * s;
		point2 = s2.from + d2 * t;

		return sqDist(point1, point2);
	}


	Circle::Circle() : Circle(Vector2(), 0) {}

	Circle::Circle(const Vector2& center, float radius) : center(center), radius(radius) {}

	bool Circle::contains(const Vector2& point) const {
		return (point - center).lengthSquared() <= sqr(radius);
	}

}