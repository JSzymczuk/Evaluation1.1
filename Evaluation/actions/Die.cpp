#include "Die.h"

DieAction::DieAction(Actor* actor) : Action(actor) {}
DieAction::~DieAction() {}
void DieAction::finish(GameTime gameTime) { }
bool DieAction::update(GameTime gameTime) { return gameTime - getTimeStarted() > ActorDyingTime; }
bool DieAction::locksRotation() const { return true; }
bool DieAction::locksMovement() const { return true; }
int DieAction::getPriority() const { return 10; }
ActionType DieAction::getActionType() const { return ActionType::DIE; }

