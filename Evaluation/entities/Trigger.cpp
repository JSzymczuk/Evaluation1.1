#include "Trigger.h"
#include "Actor.h"
#include "Missile.h"
#include "main/Configuration.h"
#include "engine/MissileManager.h"
#include "engine/Rng.h"

Vector2 GameDynamicObject::getPosition() const { return _position; }
float GameDynamicObject::getOrientation() const { return _orientation; }

Trigger::Trigger(const Vector2& position, const String& label)
	: _isActive(false), _label(label), GameDynamicObject(position, 0) {
	_activationTime = SDL_GetPerformanceCounter()
		+ MinInitialTriggerActivationTime - MinTriggerActivationTime
		+ Rng::getInteger(MinTriggerActivationTime, MaxTriggerActivationTime);
}

Trigger::~Trigger() {}

bool Trigger::isStaticElement() const { return true; }

bool Trigger::hasPositionChanged() const { return false; }

bool Trigger::isSolid() const { return false; }

Aabb Trigger::getAabb() const {
	return Aabb(_position.x - TriggerRadius, _position.y - TriggerRadius, 2 * TriggerRadius, 2 * TriggerRadius);
}

float Trigger::getRadius() const { return TriggerRadius; }

GameDynamicObjectType Trigger::getGameObjectType() const { return GameDynamicObjectType::TRIGGER; }

bool Trigger::isActive() const { return _isActive; }

void Trigger::update(GameTime time) {
	if (_isActive) {
		if (_activationTime + TriggerDeactivationTime < time) { deactivate(time); }
		else { _orientation += TriggerRotationSpeed; }
	}
	else if (_activationTime < time) { activate(time); }
}

void Trigger::deactivate(GameTime time) {
	_isActive = false;
	setNextActivationTime(time);
	//Logger::log("Trigger " + _label + " deactivated.");
}

void Trigger::activate(GameTime time) {
	_isActive = true;
	_activationTime = time;
	_orientation = 0;
	//Logger::log("Trigger " + _label + " activated.");
}

void Trigger::pick(Actor* actor, GameTime time) { deactivate(time); }

void Trigger::setNextActivationTime(GameTime time) {
	_activationTime = time + Rng::getInteger(MinTriggerActivationTime, MaxTriggerActivationTime);
}

TriggerType AmmoPack::getTriggerType() const { return TriggerType::WEAPON; }
String AmmoPack::getWeaponType() const { return _weaponName; }
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
	Trigger::pick(actor, time);
}

void ArmorPack::pick(Actor* actor, GameTime time) {
	int shots = actor->getRemainingArmorShots();
	if (shots == 0) {
		actor->setArmor(ARMOR_TRIGGER_BONUS);
		actor->setRemainingArmorShots(ARMOR_MAX_SHOTS);
		Logger::log("Actor " + actor->getName()
			+ " gained armor bonus +"
			+ std::to_string(ARMOR_TRIGGER_BONUS) + ".");
	}
	else if (ARMOR_MAX_SHOTS) {
		float armor = common::min(actor->getArmor() * (1 + ARMOR_TRIGGER_MULTIPLIER), MAX_ARMOR);
		actor->setArmor(armor);
		Logger::log("Actor " + actor->getName()
			+ " armor rose. Current armor: +"
			+ std::to_string(armor) + ".");
	}
	else {
		actor->setRemainingArmorShots(ARMOR_MAX_SHOTS);
		Logger::log("Actor " + actor->getName() + "'s armor was renewed.");
	}
	Trigger::pick(actor, time);
}

void MedPack::pick(Actor* actor, GameTime time) {
	actor->heal(MEDPACK_HEALTH_BONUS);
	Trigger::pick(actor, time);
}