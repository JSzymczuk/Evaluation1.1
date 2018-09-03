#pragma once

#include "Action.h"

struct WeaponInfo;

class ShootAction : public Action {
public:
	ShootAction(Actor* actor, const Vector2& target);
	~ShootAction();

	void start(GameTime gameTime);
	void finish(GameTime gameTime);
	bool update(GameTime gameTime);

	ActionType getActionType() const;
	int getPriority() const;
	bool locksRotation() const;
	bool locksMovement() const;

private:
	int _shots;
	Vector2 _target;
	GameTime _nextShotTime;
};
