#pragma once

#include "Action.h"

class DieAction : public Action {
public:
	DieAction(Actor* actor);
	~DieAction();

	void finish(GameTime gameTime) override;
	bool update(GameTime gameTime) override;

	bool locksRotation() const override;
	bool locksMovement() const override;
	bool isTransactional() const override;
	ActionType getActionType() const override;

};
