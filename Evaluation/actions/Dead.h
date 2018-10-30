#pragma once

#include "Action.h"

class DeadAction : public Action {
public:
	DeadAction(Actor* actor);
	~DeadAction();

	bool update(GameTime gameTime) override;

	bool locksRotation() const override;
	bool locksMovement() const override;
	bool isTransactional() const override;

	ActionType getActionType() const override;
};