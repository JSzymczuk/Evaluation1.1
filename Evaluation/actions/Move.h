#pragma once

#include "Action.h"

class MoveAction : public Action {
public:
	MoveAction(Actor* actor);
	MoveAction(Actor* actor, const Vector2& position);
	~MoveAction();
	ActionType getActionType() const;
	int getPriority() const;
	void start(GameTime gameTime);
	bool update(GameTime gameTime);

private:
	Vector2 _position;
	bool _pathplanning;
};

class MoveAtAction : public Action {
public:
	MoveAtAction(Actor* actor, const Vector2& velocity);
	~MoveAtAction();
	ActionType getActionType() const;
	int getPriority() const;
	void start(GameTime gameTime);
	bool update(GameTime gameTime);

private:
	Vector2 _velocity;
};

class WanderAction : public Action {
public:
	WanderAction(Actor* actor);
	~WanderAction();
	ActionType getActionType() const;
	int getPriority() const;
	void start(GameTime gameTime);
	bool update(GameTime gameTime);

private:
	Vector2 _center;
};