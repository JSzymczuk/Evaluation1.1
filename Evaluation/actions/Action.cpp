#include "Action.h"
#include "entities/Actor.h"
#include "Idle.h"
#include "Move.h"

Action::Action(Actor* actor) : _actor(actor), _timeStarted(0) {}

Action::~Action() {}

bool Action::hasStarted() const { return _timeStarted > 0; }

void Action::start(GameTime gameTime) { _timeStarted = gameTime; }

void Action::finish(GameTime gameTime) { 
	if (_actor->isMoving()) {
		_actor->setNextAction(new MoveAction(_actor));
	}
	else {
		_actor->setNextAction(new IdleAction(_actor)); 
	}
}

bool Action::locksRotation() const { return false; }

bool Action::locksMovement() const { return false; }

GameTime Action::getTimeStarted() const { return _timeStarted; }

Actor* Action::getActor() const { return _actor; }
