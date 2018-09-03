#pragma once

class Vector2 {
public:
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
	Vector2(const Vector2 &v);
	~Vector2();

	float get(int i) const;
	void set(int i, float value);
	float getX() const;
	void setX(float value);
	float getY() const;
	void setY(float value);

	float length() const;
	float lengthSquared() const;
	Vector2 normal() const;
	void normalize();

	bool operator==(const Vector2& v) const;
	bool operator!=(const Vector2& v) const;
	
	Vector2 operator-() const;

	Vector2 operator+(const Vector2 &v) const;
	Vector2 operator-(const Vector2 &v) const;
	Vector2 operator*(float k) const;
	Vector2 operator/(float k) const;

	Vector2 operator+=(const Vector2 &v);
	Vector2 operator-=(const Vector2 &v);
	Vector2 operator*=(float k);
	Vector2 operator/=(float k);

	float dot(const Vector2 &v) const;

	static Vector2 min(const Vector2& v1, const Vector2& v2);
	static Vector2 max(const Vector2& v1, const Vector2& v2);
};
