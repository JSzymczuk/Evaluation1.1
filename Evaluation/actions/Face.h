#pragma once

#include "Action.h"

class FaceAction : public Action {
public:
	FaceAction(Actor* actor, const Vector2& target);
	~FaceAction();
	bool locksRotation() const;
	ActionType getActionType() const;
	int getPriority() const;
	void start(GameTime gameTime);
	bool update(GameTime gameTime);
private:
	Vector2 _target;
};
