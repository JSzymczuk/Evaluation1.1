#include "TriggerFactory.h"

std::vector<Trigger*> TriggerFactory::_triggers = std::vector<Trigger*>();
bool TriggerFactory::_initialized = false;
int TriggerFactory::_created[TRIGGERS_SIZE];

Trigger* TriggerFactory::create(TriggerType type, const Vector2& position) {
	Trigger* trigger = nullptr;
	switch (type) {
	case HEALTH:
		trigger = new MedPack(position, "MedPack" + ++_created[HEALTH]);
		break;
	case WEAPON:
		trigger = new AmmoPack(Config.DefaultWeapon, position, "AmmoPack" + ++_created[WEAPON]);
		break;
	case ARMOR:
		trigger = new ArmorPack(position, "ArmorPack" + ++_created[ARMOR]);
		break;
	}
	if (trigger != nullptr) { _triggers.push_back(trigger); }
	return trigger;
}

Trigger* TriggerFactory::create(const String& type, const Vector2& position) {
	Trigger* trigger = new AmmoPack(type, position, "AmmoPack" + ++_created[WEAPON]);
	_triggers.push_back(trigger);
	return trigger;
}

void TriggerFactory::initialize() {
	if (!_initialized) {
		for (int i = 0; i < TRIGGERS_SIZE; ++i) {
			_created[i] = 0;
		}
		_initialized = true;
	}
}

void TriggerFactory::destroyAll() {
	for (Trigger* tPtr : _triggers) {
		delete tPtr;
	}
	for (int i = 0; i < TRIGGERS_SIZE; ++i) {
		_created[i] = 0;
	}
}