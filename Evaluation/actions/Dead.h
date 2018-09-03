#pragma once

#include "Action.h"

class DeadAction : public Action {
public:
	DeadAction(Actor* actor);
	~DeadAction();

	bool update(GameTime gameTime);

	bool locksRotation() const;
	bool locksMovement() const;

	int getPriority() const;
	ActionType getActionType() const;
};