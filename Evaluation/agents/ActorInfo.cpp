#pragma once

#include "ActorInfo.h"
#include "entities/Team.h"

ActorInfo::ActorInfo(Actor* actor) {
	_name = actor->getName();
	_team = actor->getTeam()->getNumber();
	_position = actor->getPosition();
	_orientation = actor->getOrientation();
	_health = actor->getHealth();
	_armor = actor->getArmor();
	_weapon = actor->getCurrentWeapon();
}

ActorInfo::~ActorInfo() {}

const char* ActorInfo::getName() const { return _name.c_str(); }

unsigned short ActorInfo::getTeam() const { return _team; }

Vector2 ActorInfo::getPosition() const { return _position; }

float ActorInfo::getOrientation() const { return _orientation; }

int ActorInfo::getHealth() const { return _health; }

int ActorInfo::getArmor() const { return _armor; }

String ActorInfo::getWeaponType() const { return _weapon; }
