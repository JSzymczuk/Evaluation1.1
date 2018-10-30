#include "Idle.h"
#include "entities/Actor.h"

IdleAction::IdleAction(Actor* actor) : Action(actor) {}

IdleAction::~IdleAction() {}

ActionType IdleAction::getActionType() const { return ActionType::IDLE; }

bool IdleAction::isTransactional() const { return false; }

bool IdleAction::update(GameTime gameTime) { return false; }

void IdleAction::start(GameTime gameTime) { 
	getActor()->stop();
	Action::start(gameTime);
}

