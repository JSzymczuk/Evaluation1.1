#include "ChangeWeapon.h"
#include "entities/Actor.h"

ChangeWeaponAction::ChangeWeaponAction(Actor* actor, const String& weaponName) : Action(actor), _weaponName(weaponName) {}

ChangeWeaponAction::~ChangeWeaponAction() {}

ActionType ChangeWeaponAction::getActionType() const { return ActionType::CHANGE_WEAPON; }

int ChangeWeaponAction::getPriority() const { return 5; }

void ChangeWeaponAction::finish(GameTime gameTime) {
	getActor()->setCurrentWeapon(_weaponName);
	Action::finish(gameTime);
}

bool ChangeWeaponAction::update(GameTime gameTime) {
	return gameTime - getTimeStarted() > WeaponChangeTime || getActor()->getCurrentWeapon() == _weaponName;
}
