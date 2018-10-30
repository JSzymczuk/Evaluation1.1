#include "Die.h"
#include "entities/Actor.h"
#include "Dead.h"

DieAction::DieAction(Actor* actor) : Action(actor) {}

DieAction::~DieAction() {}

void DieAction::finish(GameTime gameTime) { 
	Actor* actor = getActor();
	actor->setNextAction(new DeadAction(actor));
}

bool DieAction::update(GameTime gameTime) { return gameTime - getTimeStarted() > Config.ActorDyingTime * SDL_GetPerformanceFrequency(); }

bool DieAction::locksRotation() const { return true; }

bool DieAction::locksMovement() const { return true; }

bool DieAction::isTransactional() const { return true; }

ActionType DieAction::getActionType() const { return ActionType::DIE; }

