#pragma once

#include "Action.h"

class DieAction : public Action {
public:
	DieAction(Actor* actor);
	~DieAction();

	void finish(GameTime gameTime);
	bool update(GameTime gameTime);

	bool locksRotation() const;
	bool locksMovement() const;

	int getPriority() const;
	ActionType getActionType() const;
};
