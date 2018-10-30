#pragma once

#include "Action.h"

class FaceAction : public Action {
public:
	FaceAction(Actor* actor, const Vector2& target);
	~FaceAction();
	ActionType getActionType() const override;
	bool locksRotation() const override;
	bool isTransactional() const override;
	void start(GameTime gameTime) override;
	bool update(GameTime gameTime) override;

private:
	Vector2 _target;
};
