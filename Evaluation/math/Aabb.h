#pragma once

#include "Vector2.h"

// Prostok¹t o bokach równloeg³ych do osi uk³adu wspó³rzêdnych (Axis-aligned bounding box)
class Aabb {

public:
	Aabb(const Vector2& point1, const Vector2& point2);
	Aabb(float x, float y, float width, float height);
	Aabb(const Vector2& topLeft, float width, float height);
	Aabb(const Aabb& aabb);

	// Rozmiar prostok¹ta
	Vector2 getSize() const;

	float getWidth() const;
	float getHeight() const;

	//Powierzchnia prostok¹ta
	float getVolume() const;

	// Wspó³rzêdne œrodka prostok¹ta
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

	// Przesuniêcie prostok¹ta o podany wektor.
	void translate(const Vector2& vector);

	// Zmienia rozmiar prostok¹ta na podany.
	void resize(const Vector2& size, ResizeOrigin origin);

	// Skaluje prostok¹t wzglêdem jego œrodka o podany wspó³czynnik. Wartoœæ wspó³czynnika musi byæ dodatnia.
	void scale(float multiplier, ResizeOrigin origin = ResizeOrigin::CENTER);

	// £¹czy podan¹ parê Aabb. Tworzy najmniejszy Aabb, który je ogranicza.
	static Aabb merge(const Aabb& aabb1, const Aabb& aabb2);

	// Sprawdza, czy podany punkt nale¿y do prostok¹ta.
	bool contains(const Vector2& point) const;

	// Sprawdza, czy podany prostok¹t znajduje siê wewn¹trz prostok¹ta.
	bool contains(const Aabb& aabb) const;

	// Sprawdza, czy prostok¹t przecina siê z podanym.
	bool intersects(const Aabb& aabb) const;

private:
	Vector2 _minPoint;
	Vector2 _maxPoint;

};
