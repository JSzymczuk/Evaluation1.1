#include "entities/Trigger.h"
#include "entities/Actor.h"
#include "entities/Missile.h"
#include "main/Configuration.h"
#include "engine/MissileManager.h"
#include "engine/Rng.h"

Trigger::Trigger(const Vector2& position, const String& label)
	: _isActive(false), _label(label), DynamicEntity(position, 0) {
	_activationTime = SDL_GetPerformanceCounter()
		+ Config.MinInitialTriggerActivationTime - Config.MinTriggerActivationTime
		+ Rng::getInteger(Config.MinTriggerActivationTime, Config.MaxTriggerActivationTime);
}

Trigger::~Trigger() {}
 
bool Trigger::isSolid() const { return false; }

bool Trigger::isSpotting() const { return false; }

float Trigger::getRadius() const { return Config.TriggerRadius; }

bool Trigger::isActive() const { return _isActive; }

void Trigger::onCollision(CollisionInvoker* invoker, GameTime time) {
	Actor* actor = dynamic_cast<Actor*>(invoker);
	if (actor != nullptr && isActive()) { 
		pick(actor, time); 
		deactivate(time);
	}
}

void Trigger::update(GameTime time) {
	if (_isActive) {
		if (_activationTime + Config.TriggerDeactivationTime < time) { deactivate(time); }
		else { _orientation += Config.TriggerRotationSpeed; }
	}
	else if (_activationTime < time) { activate(time); }
}

void Trigger::deactivate(GameTime time) {
	_isActive = false;
	setNextActivationTime(time);
}

void Trigger::activate(GameTime time) {
	_isActive = true;
	_activationTime = time;
	_orientation = 0;
}

void Trigger::setNextActivationTime(GameTime time) {
	_activationTime = time + Rng::getInteger(Config.MinTriggerActivationTime, Config.MaxTriggerActivationTime);
}

String AmmoPack::getName() const { return _weaponName; }
String ArmorPack::getName() const { return Config.ArmorPackName; }
String MedPack::getName() const { return Config.MedPackName; }

String AmmoPack::getWeaponType() const { return _weaponName; }
TriggerType AmmoPack::getTriggerType() const { return TriggerType::WEAPON; }
TriggerType ArmorPack::getTriggerType() const { return TriggerType::ARMOR; }
TriggerType MedPack::getTriggerType() const { return TriggerType::HEALTH; }

AmmoPack::AmmoPack(const String& weaponName, const Vector2& position, const String& label)
	: Trigger(position, label), _weaponName(weaponName) {}
ArmorPack::ArmorPack(const Vector2& position, const String& label) : Trigger(position, label) {}
MedPack::MedPack(const Vector2& position, const String& label) : Trigger(position, label) {}

void AmmoPack::pick(Actor* actor, GameTime time) {
	int currentAmmo = actor->getWeaponState(_weaponName).ammo;
	auto weaponInfo = getWeaponInfo(_weaponName);
	int newAmmo = currentAmmo + weaponInfo.packAmmo > weaponInfo.maxAmmo ?
		weaponInfo.maxAmmo : currentAmmo + weaponInfo.packAmmo;
	actor->setAmmo(_weaponName, newAmmo);
	newAmmo -= currentAmmo;
	if (newAmmo > 0) {
		Logger::log("Actor " + actor->getName() + " picked "
			+ std::to_string(newAmmo) + " ammo for "
			+ weaponInfo.name + ".");
	}
	else {
		Logger::log("Aktor " + actor->getName() + " ammo for "
			+ weaponInfo.name + ".");
	}
}

void ArmorPack::pick(Actor* actor, GameTime time) {
	int shots = actor->getRemainingArmorShots();
	if (shots == 0) {
		actor->setArmor(Config.ArmorTriggerBonus);
		actor->setRemainingArmorShots(Config.ArmorMaxShots);
		Logger::log("Actor " + actor->getName()
			+ " gained armor bonus +"
			+ std::to_string(Config.ArmorTriggerBonus) + ".");
	}
	else if (Config.ArmorMaxShots) {
		float armor = common::min(actor->getArmor() * (1 + Config.ArmorTriggerMultiplier), Config.MaxArmor);
		actor->setArmor(armor);
		Logger::log("Actor " + actor->getName()
			+ " armor rose. Current armor: +"
			+ std::to_string(armor) + ".");
	}
	else {
		actor->setRemainingArmorShots(Config.ArmorMaxShots);
		Logger::log("Actor " + actor->getName() + "'s armor was renewed.");
	}
}

void MedPack::pick(Actor* actor, GameTime time) {
	actor->heal(Config.MedpackHealthBonus);
}