#include "Face.h"
#include "entities/Actor.h"

FaceAction::FaceAction(Actor* actor, const Vector2& target) : Action(actor), _target(target) {}

FaceAction::~FaceAction() {}

ActionType FaceAction::getActionType() const { return ActionType::FACE; }

int FaceAction::getPriority() const { return 0; }

bool FaceAction::locksRotation() const { return true; }

void FaceAction::start(GameTime gameTime) {
	getActor()->lookAt(_target); 
	Action::start(gameTime);
}

bool FaceAction::update(GameTime gameTime) { 
	Actor* actor = getActor();
	actor->lookAt(_target);
	return true; 
}
