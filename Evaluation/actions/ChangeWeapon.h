#pragma once

#include "Action.h"

class ChangeWeaponAction : public Action {
public:
	ChangeWeaponAction(Actor* actor, const String& weaponName);
	~ChangeWeaponAction();
	ActionType getActionType() const;
	int getPriority() const;
	void finish(GameTime gameTime);
	bool update(GameTime gameTime);

private:
	String _weaponName;
};
