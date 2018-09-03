#include "assert.h"
#include "Vector2.h"
#include "math.h"

Vector2::Vector2() : Vector2::Vector2(0, 0) {}
Vector2::Vector2(const Vector2& v) : Vector2(v.x, v.y) {}
Vector2::Vector2(float x, float y) : x(x), y(y) {}
Vector2::~Vector2() { }

float Vector2::get(int i) const {
	if (i == 0) { return x; }
	if (i == 1) { return y; }
	throw;
}

void Vector2::set(int i, float value) {
	if (i == 0) { x = value; }
	else if (i == 1) { y = value; }
	else throw;
}

float Vector2::getX() const { return x; }
void Vector2::setX(float value) { x = value; }
float Vector2::getY() const { return y; }
void Vector2::setY(float value) { y = value; }

Vector2 Vector2::normal() const {
	float sqLth = lengthSquared();
	if (sqLth <= common::EPSILON) {
		return Vector2();
	}
	return (*this / sqrtf(sqLth));
}

void Vector2::normalize() {
	float sqLth = lengthSquared();
	if (sqLth > common::EPSILON) {
		*this = (*this / sqrtf(sqLth));
	}
}

float Vector2::length() const {
	return sqrtf(lengthSquared());
}

#define sqr(x) (x * x)

float Vector2::lengthSquared() const {
	return sqr(x) + sqr(y);
}

float Vector2::dot(const Vector2 &v) const {
	return x * v.x + y * v.y;
};

Vector2 Vector2::operator+(const Vector2 &v) const {
	return Vector2(x + v.x, y + v.y);
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

Vector2 Vector2::operator-(const Vector2 &v) const {
	return Vector2(x - v.x, y - v.y);
}

Vector2 Vector2::operator*(float k) const {
	return Vector2(x * k, y * k);
}

Vector2 Vector2::operator/(float k) const {
	assert(k != 0.0f);
	return Vector2(x / k, y / k);
}

Vector2 Vector2::operator+=(const Vector2 &v) {
	x += v.x;
	y += v.y;
	return *this;
}

Vector2 Vector2::operator-=(const Vector2 &v) {
	x -= v.x;
	y -= v.y;
	return *this;
}

Vector2 Vector2::operator*=(float k) {
	x *= k;
	y *= k;
	return *this;
}

Vector2 Vector2::operator/=(float k) {
	assert(k != 0.0f);
	x /= k;
	y /= k;
	return *this;
}

bool Vector2::operator==(const Vector2& v) const {
	return (x == v.x) && (y == v.y);
}

bool Vector2::operator!=(const Vector2& v) const {
	return (x != v.x) || (y != v.y);
}

Vector2 Vector2::min(const Vector2& v1, const Vector2& v2) {
	return Vector2(fminf(v1.x, v2.x), fminf(v1.y, v2.y));
}

Vector2 Vector2::max(const Vector2& v1, const Vector2& v2) {
	return Vector2(fmaxf(v1.x, v2.x), fmaxf(v1.y, v2.y));
}
