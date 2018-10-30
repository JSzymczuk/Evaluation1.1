#pragma once

#include "math/Math.h"
#include "main/Configuration.h"
#include <vector>
#include <map>

class Actor;
class Team;
class CollisionResolver;
class CollisionInvoker;

class Updatable {
public:
	virtual void update(GameTime gameTime) = 0;
};

class CollisionResponder {
public:
	virtual void onCollision(CollisionInvoker* actor, GameTime time) = 0;
};

class CollisionInvoker { 
public:
	virtual void invokeCollision(CollisionResponder* responder, GameTime time);
	virtual void invokeCollision(std::vector<CollisionResponder*> responders, GameTime time);
};

class Destructible {
public:
	virtual String getName() const = 0;
	virtual Team* getTeam() const = 0;
	virtual float recieveDamage(float damage) = 0;
	virtual bool wasDestroyed() const = 0;
	virtual bool isDestroyed() const = 0;
	virtual void onDestroy() = 0;
	virtual float getSquareDistanceTo(const Vector2& point) const = 0;
};

class Spottable {
public:
	virtual bool isSolid() const = 0;
	virtual bool isSpotting() const = 0;
	virtual Vector2 getPosition() const = 0;
	virtual float getRadius() const = 0;
};

class Spotter : public virtual Updatable, 
	public virtual Spottable {
public:
	virtual float getSightRadius() const = 0;
	virtual bool hasPositionChanged() const = 0;

	bool isSpotting() const override;
	void update(GameTime gameTime) override;

	void spot(Spottable* entity);
	void unspot(Spottable* entity);
	std::vector<Spottable*> getSpottedObjects() const;

protected: 
	virtual CollisionResolver* getCollisionResolver() const = 0;

private: 
	std::vector<Spottable*> getNearbyObjects() const;
	std::vector<Spottable*> _spottedObjects;
};

class Entity { };

//struct EntityDiagnostics {
//	GameTime total;
//	GameTime logic;
//	GameTime update;
//	GameTime broadphase;
//	GameTime regionsCircle;
//	GameTime regionsSegment;
//	GameTime unitTests;
//	GameTime mutexLock;
//	GameTime mutexUnlock;
//};

class StaticEntity : public Entity {
public:
	StaticEntity(const Aabb& aabb);

	virtual std::vector<Segment> getBounds() const = 0;
	virtual size_t getBoundsSize() const = 0;

	Aabb getAabb() const;
	bool isStaticElement() const;

private:
	Aabb _aabb;
};

class DynamicEntity : public Entity, 
	public virtual Updatable, 
	public virtual Spottable {
public:
	DynamicEntity();
	DynamicEntity(const Vector2& position, float orientation);
	virtual ~DynamicEntity();

	Vector2 getPosition() const override;
	float getOrientation() const;

	virtual bool isSolid() const = 0;

	//mutable EntityDiagnostics diagnostics;

	Aabb getAabb() const;
	virtual bool isStaticElement() const;

protected:
	Vector2 _position;
	float _orientation;

	CollisionResolver* getCollisionResolver() const;
	
private:
	CollisionResolver* _collisionResolver = nullptr;

	void setCollisionResolver(CollisionResolver* collisionResolver);
	void unsetCollisionResolver();

	friend class CollisionResolver;
};

bool checkCollision(const StaticEntity* entity, const Segment& segment);
float getDistanceTo(const StaticEntity* entity, const Vector2& point);
float getSqDistanceTo(const StaticEntity* entity, const Segment& segment);
