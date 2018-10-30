#include "ChangeWeapon.h"
#include "entities/Actor.h"

ChangeWeaponAction::ChangeWeaponAction(Actor* actor, const String& weaponName) : Action(actor), _weaponName(weaponName) {}

ChangeWeaponAction::~ChangeWeaponAction() {}

ActionType ChangeWeaponAction::getActionType() const { return ActionType::CHANGE_WEAPON; }

bool ChangeWeaponAction::isTransactional() const { return true; }

void ChangeWeaponAction::finish(GameTime gameTime) {
	getActor()->setCurrentWeapon(_weaponName);
	Action::finish(gameTime);
}

bool ChangeWeaponAction::update(GameTime gameTime) {
	return gameTime - getTimeStarted() > Config.WeaponChangeTime || getActor()->getCurrentWeapon() == _weaponName;
}
