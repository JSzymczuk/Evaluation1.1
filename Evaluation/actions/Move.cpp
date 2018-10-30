#include "Move.h"
#include "entities/Actor.h"
#include "main/Game.h"

MoveAction::MoveAction(Actor* actor) : Action(actor), _pathplanning(false) {}

MoveAction::MoveAction(Actor* actor, const Vector2& position) : Action(actor), _position(position), _pathplanning(true) {}

MoveAction::~MoveAction() {}

ActionType MoveAction::getActionType() const { return ActionType::MOVE; }

bool MoveAction::isTransactional() const { return false; }

bool MoveAction::update(GameTime gameTime) {
	return !getActor()->isMoving(); 
}

void MoveAction::start(GameTime gameTime) {
	if (_pathplanning) {
		Actor* actor = getActor();
		actor->move(Game::getInstance()->getMap()->findPath(actor->getPosition(), _position, actor));
		_pathplanning = false;
	}
	Action::start(gameTime);
}

MoveAtAction::MoveAtAction(Actor* actor, const Vector2& velocity) : Action(actor), _velocity(velocity) {}

MoveAtAction::~MoveAtAction() {}

ActionType MoveAtAction::getActionType() const { return ActionType::MOVE; }

bool MoveAtAction::isTransactional() const { return false; }

bool MoveAtAction::update(GameTime gameTime) { return getActor()->isMoving(); }

void MoveAtAction::start(GameTime gameTime) {
	Actor* actor = getActor();
	actor->stop();
	actor->setPreferredVelocity(_velocity.normal() * actor->getMaxSpeed());
	Action::start(gameTime);
}


WanderAction::WanderAction(Actor* actor) : Action(actor) {}

WanderAction::~WanderAction() {}

ActionType WanderAction::getActionType() const { return ActionType::WANDER; }

bool WanderAction::isTransactional() const { return false; }

void WanderAction::start(GameTime gameTime) {
	Actor* actor = getActor();
	actor->stop();
	_center = actor->getPosition();
	Logger::log("Actor " + actor->getName() + " started wandering.");
	Action::start(gameTime);
}

bool WanderAction::update(GameTime gameTime) {
	Actor* actor = getActor();
	float ang = !actor->isWaiting() ? common::angle(actor->getVelocity()) + Rng::getFloat(-Config.ActorWanderSpread, Config.ActorWanderSpread)
		: common::PI_F + actor->getOrientation();
	Vector2 prefVelocity = Vector2(cosf(ang), sinf(ang));
	Vector2 returnVel = _center - actor->getPosition();
	float lth = returnVel.lengthSquared();
	float k = common::max(0, common::min(1, lth / (common::sqr(Config.WanderCentripetalForce))));
	actor->setPreferredVelocity((returnVel * k + prefVelocity * (1 - k)) * getActor()->getMaxSpeed());
	return false;
}