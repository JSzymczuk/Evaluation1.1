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
	DEAD
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

	// std::string getActionCode() { return actionCode; }
	//bool isRepeatable() { return repeatable; }

	//GameTime getActionStarted() { return actionStarted; }
	//Action* getNextAction() { return nextAction; }
	//void setNextAction(Action * action) { this->nextAction = action; }
	//ActionType getActionType() { return actionType; }
	
protected:
	//ActionType actionType;
	//void rotate(Actor * actor, Vector4d direction, GameTime time);
	//const double cosOfPiBy8;
	//std::string actionCode;
	//bool breakable;
	//bool repeatable;
	//TimeUnit standardCost;
	//Action * nextAction;

	Actor* getActor() const;
	GameTime getTimeStarted() const;

private:
	Actor* _actor;
	GameTime _timeStarted;

};
