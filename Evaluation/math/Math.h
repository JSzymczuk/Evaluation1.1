#pragma once

#include <math.h>
#include <utility>
#include "Segment.h"

namespace common {

	const float PI_F = 3.14159265358979f;
	const float PI_2_F = PI_F / 2;
	const float PI_8_F = PI_F / 8;
	const float EPSILON = 0.0001f;

	template<typename T> void swap(T& x, T& y) {
		T temp = x;
		x = y;
		y = temp;
	}

	inline float min(float x, float y) { return x < y ? x : y; }
	inline float max(float x, float y) { return x > y ? x : y; }
	inline float abs(float x) { return x < 0 ? -x : x; }
	inline float sqr(float x) { return x * x; }
	inline int sign(float x) { return x > EPSILON ? 1 : x < -EPSILON ? -1 : 0; }

	inline float degrees(float radians) { return radians * 180.f / PI_F; }
	inline float radians(float degrees) { return degrees * PI_F / 180.f; }
	
	class Circle {
	public:
		Vector2 center;
		float radius;

		Circle();
		Circle(const Vector2& center, float radius);
		bool contains(const Vector2& point) const;
	};

	struct Ring {
		Vector2 center;
		float radius1;
		float radius2;
	};

	inline float sqDist(const Vector2& p1, const Vector2& p2) {
		Vector2 q = p2 - p1;
		return q.x * q.x + q.y * q.y;
	}

	inline float distance(const Vector2& p1, const Vector2& p2) { return sqrtf(sqDist(p1, p2)); }
	
	inline float angle(const Vector2& v) { return atan2f(v.y, v.x); }

	inline float angleFromTo(const Vector2& from, const Vector2& to) { return angle(to - from); }

	// Dwuwymiarowy iloczyn wektorowy wektorów v oraz w.
	inline float cross(const Vector2& v1, const Vector2& v2) { return v1.x * v2.y - v1.y * v2.x; }

	inline float clamp(float val, float min, float max) { return val > max ? max : val < min ? min : val; }

	// Zwraca wektor ustalonej d³ugoœci (je¿eli podany nie jest zerowy).
	inline Vector2 adjustLength(const Vector2& vec, float length) { return vec.normal() * length; }
	
	// Przycina wektor do podanej d³ugoœci (je¿eli by³ d³u¿szy).
	Vector2 truncate(const Vector2& vec, float length);

	// Je¿eli poruszamy siê po obwodzie trójk¹ta kolejno przechodz¹c przez podane punkty,
	// a wnêtrze trójk¹ta pozostaje z prawej strony, to orientacja trójk¹ta jest dodatnia.
	inline int triangleOrientation(const Vector2& point1, const Vector2& point2, const Vector2& point3) {
		// Iloczyn wektorowy wektorów (p2-p1) i z (p3-p1).
		return sign(point1.y * point3.x + point3.y * point2.x + point2.y * point1.x
			- point1.x * point3.y - point3.x * point2.y - point2.x * point1.y);
	}

	// Zwraca wspó³rzêdne punktu przeciêcia odcinków sparametryzowanych:
	// p1 + t * (q1 - p1) oraz p2 + u * (q2 - p2), gdzie t, u in [0; 1].
	bool testSegments(const Vector2& p1, const Vector2& q1, const Vector2& p2, const Vector2& q2, Vector2& result);

	// Zwraca wspó³rzêdne punktu przeciêcia odcinków sparametryzowanych segment1 oraz segment2.
	bool testSegments(const Segment& segment1, const Segment& segment2, Vector2& result);

	bool testCircles(const Circle& circle1, const Circle& circle2);

	bool testCircleAndSegment(const Circle& circle, const Segment& segment);

	Vector2 rotatePoint(const Vector2& point, const Vector2& origin, float angle);

	Vector2 rotatePoint(const Vector2& point, const Vector2& origin, float cosAngle, float sinAngle);

	// Zwraca ca³y fragment wewn¹trz obszaru box prostej zawieraj¹cej odcinek segment. 
	Segment extendSegment(const Segment& segment, const Aabb& box);

	// Rzut ortogonalny wektora v1 na wektor v2
	inline Vector2 projection(const Vector2& v1, const Vector2& v2) { return v2 * (v1.dot(v2) / v2.dot(v2)); }

	// Odleg³oœæ punktu od odcinka.
	float distance(const Vector2& point, const Segment& segment);

	// Normalizuje k¹t do przedzia³u [-pi; pi).
	inline float normalizeAngle(float angle) { return fmodf(angle + 3 * common::PI_F, 2 * common::PI_F) - common::PI_F; }

	float measureAngle(float from, float to);

	bool isAngleBetween(float angle, float angle1, float angle2);

	float sqDist(const Segment& s1, const Segment& s2);

	float sqDist(const Segment& s1, const Segment& s2, Vector2& point1, Vector2& point2);

	
}