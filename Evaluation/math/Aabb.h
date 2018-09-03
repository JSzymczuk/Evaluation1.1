#pragma once

#include "Vector2.h"

// Prostok�t o bokach r�wnloeg�ych do osi uk�adu wsp�rz�dnych (Axis-aligned bounding box)
class Aabb {

public:
	Aabb(const Vector2& point1, const Vector2& point2);
	Aabb(float x, float y, float width, float height);
	Aabb(const Vector2& topLeft, float width, float height);
	Aabb(const Aabb& aabb);

	// Rozmiar prostok�ta
	Vector2 getSize() const;

	float getWidth() const;
	float getHeight() const;

	//Powierzchnia prostok�ta
	float getVolume() const;

	// Wsp�rz�dne �rodka prostok�ta
	Vector2 getCenter() const;

	float getTop() const;
	float getBottom() const;
	float getLeft() const;
	float getRight() const;

	Vector2 getTopLeft() const;
	Vector2 getBottomRight() const;
	Vector2 getTopRight() const;
	Vector2 getBottomLeft() const;

	enum ResizeOrigin {
		CENTER,
		TOP_LEFT
	};

	// Przesuni�cie prostok�ta o podany wektor.
	void translate(const Vector2& vector);

	// Zmienia rozmiar prostok�ta na podany.
	void resize(const Vector2& size, ResizeOrigin origin);

	// Skaluje prostok�t wzgl�dem jego �rodka o podany wsp�czynnik. Warto�� wsp�czynnika musi by� dodatnia.
	void scale(float multiplier, ResizeOrigin origin = ResizeOrigin::CENTER);

	// ��czy podan� par� Aabb. Tworzy najmniejszy Aabb, kt�ry je ogranicza.
	static Aabb merge(const Aabb& aabb1, const Aabb& aabb2);

	// Sprawdza, czy podany punkt nale�y do prostok�ta.
	bool contains(const Vector2& point) const;

	// Sprawdza, czy podany prostok�t znajduje si� wewn�trz prostok�ta.
	bool contains(const Aabb& aabb) const;

	// Sprawdza, czy prostok�t przecina si� z podanym.
	bool intersects(const Aabb& aabb) const;

private:
	Vector2 _minPoint;
	Vector2 _maxPoint;

};
