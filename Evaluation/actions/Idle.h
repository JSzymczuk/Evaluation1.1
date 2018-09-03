#pragma once

#include "Action.h"

class IdleAction : public Action {
public:
	IdleAction(Actor* actor);
	~IdleAction();
	ActionType getActionType() const;
	int getPriority() const;
	void start(GameTime gameTime);
	bool update(GameTime gameTime);
};
