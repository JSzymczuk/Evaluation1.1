#pragma once

#include "agents/ObjectInfo.h"
#include "entities/Actor.h"
#include "entities/Trigger.h"
#include "entities/Team.h"


ObjectInfo::ObjectInfo(GameTime time) : _time(time) {}

ObjectInfo::~ObjectInfo() {}

GameTime ObjectInfo::getObservationTime() const { return _time; }


ActorInfo::ActorInfo(const Actor* actor, GameTime time) : ObjectInfo(time) {
	_name = actor->getName();
	_team = actor->getTeam()->getNumber();
	_position = actor->getPosition();
	_orientation = actor->getOrientation();
	_health = actor->getHealth();
	_armor = actor->getArmor();
	_weapon = actor->getCurrentWeapon();
}

ActorInfo::~ActorInfo() {}

String ActorInfo::getName() const { return _name.c_str(); }

unsigned short ActorInfo::getTeam() const { return _team; }

Vector2 ActorInfo::getPosition() const { return _position; }

float ActorInfo::getOrientation() const { return _orientation; }

int ActorInfo::getHealth() const { return _health; }

int ActorInfo::getArmor() const { return _armor; }

String ActorInfo::getWeaponType() const { return _weapon; }

bool ActorInfo::isDead() const { return _health <= 0; }


TriggerInfo::TriggerInfo(const Trigger* trigger, GameTime time) : ObjectInfo(time) {
	if (trigger != nullptr) {
		_name = trigger->getName();
		/*if (type == TriggerType::HEALTH) {
			_name = Config.MedPackName;
		}
		else if (type == TriggerType::ARMOR) {
			_name = Config.ArmorPackName;
		}
		else {
			_name = ((AmmoPack*)trigger)->getWeaponType();
		}*/
		_id = trigger->getId();
		_isActive = trigger->isActive();
		_position = trigger->getPosition();
	}
}

TriggerInfo::~TriggerInfo() {}

int TriggerInfo::getId() const { return _id; }

String TriggerInfo::getName() const { return _name; }

Vector2 TriggerInfo::getPosition() const { return _position; }

bool TriggerInfo::isActive() const { return _isActive; }