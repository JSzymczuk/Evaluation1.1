#pragma once

#include "main/Configuration.h"
#include "math/Math.h"

enum ActionType {
	IDLE,
	MOVE,
	FACE,
	SHOOT,
	CHANGE_WEAPON,
	DIE,
	DEAD,
	WANDER
};

class Actor;

class Action {
public:
	Action(Actor* actor);
	virtual ~Action() = 0;

	virtual void start(GameTime gameTime);
	virtual void finish(GameTime gameTime);
	virtual bool update(GameTime gameTime) = 0;

	virtual bool locksRotation() const;
	virtual bool locksMovement() const;

	virtual int getPriority() const = 0;
	virtual ActionType getActionType() const = 0;

	bool hasStarted() const;

protected:
	Actor* getActor() const;
	GameTime getTimeStarted() const;

private:
	Actor* _actor;
	GameTime _timeStarted;
};
