#include "Move.h"
#include "entities/Actor.h"
#include "main/Game.h"

MoveAction::MoveAction(Actor* actor) : Action(actor), _pathplanning(false) {}

MoveAction::MoveAction(Actor* actor, const Vector2& position) : Action(actor), _position(position), _pathplanning(true) {}

MoveAction::~MoveAction() {}

ActionType MoveAction::getActionType() const { return ActionType::MOVE; }

int MoveAction::getPriority() const { return 0; }

bool MoveAction::update(GameTime gameTime) {
	return !getActor()->isMoving(); 
}

void MoveAction::start(GameTime gameTime) {
	if (_pathplanning) {
		Actor* actor = getActor();
		actor->move(Game::getInstance()->getMap()->findPath(actor->getPosition(), _position));
		_pathplanning = false;
	}
}

MoveAtAction::MoveAtAction(Actor* actor, const Vector2& velocity) : Action(actor), _velocity(velocity) {}

MoveAtAction::~MoveAtAction() {}

ActionType MoveAtAction::getActionType() const { return ActionType::MOVE; }

int MoveAtAction::getPriority() const { return 1; }

bool MoveAtAction::update(GameTime gameTime) { return getActor()->isMoving(); }

void MoveAtAction::start(GameTime gameTime) {
	Actor* actor = getActor();
	actor->stop();
	actor->setPreferredVelocity(_velocity.normal() * actor->getMaxSpeed());
}
