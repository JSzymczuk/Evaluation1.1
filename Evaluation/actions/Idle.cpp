#include "Idle.h"
#include "entities/Actor.h"

IdleAction::IdleAction(Actor* actor) : Action(actor) {}

IdleAction::~IdleAction() {}

ActionType IdleAction::getActionType() const { return ActionType::IDLE; }

int IdleAction::getPriority() const { return 0; }

bool IdleAction::update(GameTime gameTime) { return false; }

void IdleAction::start(GameTime gameTime) { 
	getActor()->stop();
	Action::start(gameTime);
}

