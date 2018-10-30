#pragma once

#include "Action.h"

class ChangeWeaponAction : public Action {
public:
	ChangeWeaponAction(Actor* actor, const String& weaponName);
	~ChangeWeaponAction();
	ActionType getActionType() const override;
	bool isTransactional() const override;
	void finish(GameTime gameTime) override;
	bool update(GameTime gameTime) override;

private:
	String _weaponName;
};
