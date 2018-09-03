#include "Shoot.h"
#include "entities/Actor.h"
#include "entities/Weapon.h"
#include "engine/MissileManager.h"
#include "main/Game.h"

ShootAction::ShootAction(Actor* actor, const Vector2& target) : Action(actor), _target(target), _shots(0) {}
ShootAction::~ShootAction() {}

void ShootAction::start(GameTime gameTime) {
	Action::start(gameTime);
	_nextShotTime = 0;
	getActor()->lookAt(_target);
}

void ShootAction::finish(GameTime gameTime) {
	Action::finish(gameTime);
	Actor* actor = getActor();
	WeaponState& weaponState = actor->getWeaponState(actor->getCurrentWeapon());
	weaponState.state = WeaponLoadState::WEAPON_UNLOADED;
	weaponState.lastShot = gameTime;
}

bool ShootAction::update(GameTime gameTime) {
	Actor* actor = getActor();

	String currentWeapon = actor->getCurrentWeapon();
	WeaponState& weaponState = actor->getWeaponState(currentWeapon);
	const WeaponInfo& weaponInfo = getWeaponInfo(currentWeapon);
	bool shotFailed = false;

	actor->lookAt(_target);

	if (_nextShotTime > 0 && gameTime >= _nextShotTime) {
		// Aktor patrzy w kierunku, w którym bêdzie strzela³,
		// nie wykonano jeszcze wszystkich strza³ów wchodz¹cych w sk³ad serii,
		// a czas niezbêdny do wykonania strza³u ju¿ min¹³
		if (weaponState.ammo > 0) {
			if (weaponState.state != WeaponLoadState::WEAPON_UNLOADED) {
				if (_shots == 0) {
					Logger::log("Actor " + actor->getName() + " shot a " + weaponInfo.name + ".");
				}
				++_shots;
				_nextShotTime = gameTime + weaponInfo.shotTime;
				weaponState.ammo--;
				weaponState.lastShot = gameTime;
				weaponState.state = WeaponLoadState::WEAPON_SHOOTING;
				Game::getInstance()->getMissileManager()->shootAt(actor, _target, gameTime);
			}
			else {
				shotFailed = true;
				Logger::log("Actor " + actor->getName() + " tried to shoot a " + weaponInfo.name + " but it's not loaded.");
			}
		}
		else {
			shotFailed = true;
			Logger::log("Actor " + actor->getName() + " tried to shoot a " + weaponInfo.name + " but it has no ammo left.");
		}
	}
	else if (_nextShotTime == 0 && common::abs(common::measureAngle(actor->getOrientation(),
		common::angle(_target - actor->getPosition()))) < common::PI_8_F) {
		// Nie oddano jeszcze strza³u, a k¹t jest ju¿ dostatecznie ma³y, aby zacz¹æ strzelaæ
		_nextShotTime = gameTime + weaponInfo.shotTime;
	}

	return shotFailed || _shots >= weaponInfo.seriesLength;
}

ActionType ShootAction::getActionType() const { return ActionType::SHOOT; }
int ShootAction::getPriority() const { return 0; }
bool ShootAction::locksRotation() const { return true; }
bool ShootAction::locksMovement() const { return true; }