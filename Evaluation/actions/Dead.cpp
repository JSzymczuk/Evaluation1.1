#include "Dead.h"

DeadAction::DeadAction(Actor* actor) : Action(actor) {}

DeadAction::~DeadAction() {}

bool DeadAction::update(GameTime gameTime) { return false; }

bool DeadAction::locksRotation() const { return true; }

bool DeadAction::locksMovement() const { return true; }

bool DeadAction::isTransactional() const { return true; }

ActionType DeadAction::getActionType() const { return ActionType::DEAD; }