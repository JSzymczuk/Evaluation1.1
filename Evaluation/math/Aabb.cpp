#include "Aabb.h"

Aabb::Aabb(const Vector2& point1, const Vector2& point2) {
	_minPoint = Vector2::min(point1, point2);
	_maxPoint = Vector2::max(point1, point2);
}

Aabb::Aabb(float x, float y, float width, float height) : Aabb(Vector2(x, y), width, height) { }

Aabb::Aabb(const Vector2& topLeft, float width, float height) {
	if (width < 0) { throw "Wartoœæ parametru 'width' nie mo¿e byæ ujemna."; }
	if (height < 0) { throw "Wartoœæ parametru 'height' nie mo¿e byæ ujemna."; }

	_minPoint = topLeft;
	_maxPoint = topLeft + Vector2(width, height);
}

Aabb::Aabb(const Aabb& aabb) {
	_minPoint = aabb._minPoint;
	_maxPoint = aabb._maxPoint;
}

Vector2 Aabb::getSize() const { return _maxPoint - _minPoint; }
Vector2 Aabb::getCenter() const { return (_minPoint + _maxPoint) / 2; }
float Aabb::getWidth() const { return _maxPoint.x - _minPoint.x; }
float Aabb::getHeight() const { return _maxPoint.y - _minPoint.y; }
float Aabb::getVolume() const { return (_maxPoint.x - _minPoint.x) * (_maxPoint.y - _minPoint.y); }

float Aabb::getTop() const { return _minPoint.y; }
float Aabb::getBottom() const { return _maxPoint.y; }
float Aabb::getLeft() const { return _minPoint.x; }
float Aabb::getRight() const { return _maxPoint.x; }

Vector2 Aabb::getTopLeft() const { return _minPoint; }
Vector2 Aabb::getBottomRight() const { return _maxPoint; }
Vector2 Aabb::getTopRight() const { return Vector2(_maxPoint.x, _minPoint.y); }
Vector2 Aabb::getBottomLeft() const { return Vector2(_minPoint.x, _maxPoint.y); }

void Aabb::translate(const Vector2& vector) {
	_minPoint += vector;
	_maxPoint += vector;
}

void Aabb::resize(const Vector2& size, ResizeOrigin origin = ResizeOrigin::TOP_LEFT) {
	if (size.x < 0 || size.y < 0) { throw "Rozmiar nie mo¿e byæ ujemny."; }

	switch (origin) {
	case ResizeOrigin::TOP_LEFT:
		_maxPoint = _minPoint + size;
		break;
	case ResizeOrigin::CENTER: {
		Vector2 halfSize = size / 2;
		Vector2 center = getCenter();
		_minPoint = center - halfSize;
		_maxPoint = center + halfSize;
		break;
	}
	default: break;
	}
}

void Aabb::scale(float multiplier, ResizeOrigin origin) {
	if (multiplier <= 0) { throw "Wartoœæ wspó³czynnika musi byæ dodatnia."; }
	resize(getSize() * multiplier, origin);
}

Aabb Aabb::merge(const Aabb& aabb1, const Aabb& aabb2) {
	return Aabb(
		Vector2::min(aabb1._minPoint, aabb2._minPoint),
		Vector2::max(aabb1._maxPoint, aabb2._maxPoint)
	);
}

bool Aabb::contains(const Vector2& point) const {
	return _minPoint.x <= point.x && point.x <= _maxPoint.x
		&& _minPoint.y <= point.y && point.y <= _maxPoint.y;
}

bool Aabb::contains(const Aabb& aabb) const {
	return _minPoint.x <= aabb._minPoint.x && aabb._maxPoint.x <= _maxPoint.x
		&& _minPoint.y <= aabb._minPoint.y && aabb._maxPoint.y <= _maxPoint.y;
}

bool Aabb::intersects(const Aabb& aabb) const {
	if (_maxPoint.x < aabb._minPoint.x || _minPoint.x > aabb._maxPoint.x
		|| _maxPoint.y < aabb._minPoint.y || _minPoint.y > aabb._maxPoint.y) {
		return false;
	}
	return true;
}