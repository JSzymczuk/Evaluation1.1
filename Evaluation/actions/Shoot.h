#pragma once

#include "Action.h"

struct WeaponInfo;

class ShootAction : public Action {
public:
	ShootAction(Actor* actor, const Vector2& target);
	~ShootAction();

	void start(GameTime gameTime) override;
	void finish(GameTime gameTime) override;
	bool update(GameTime gameTime) override;

	ActionType getActionType() const override;
	bool locksRotation() const override;
	bool locksMovement() const override;
	bool isTransactional() const override;

private:
	int _shots;
	Vector2 _target;
	GameTime _nextShotTime;
};
