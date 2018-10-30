#pragma once

#include "Action.h"

class IdleAction : public Action {
public:
	IdleAction(Actor* actor);
	~IdleAction();
	ActionType getActionType() const override;
	bool isTransactional() const override;
	void start(GameTime gameTime) override;
	bool update(GameTime gameTime) override;
};
