#pragma once

#include "DataLoader.h"
#include "entities/Weapon.h"
#include "math/Math.h"

class WeaponLoader : public DataLoader<WeaponInfo> {
public:
	bool load(const PropertyValues& data, WeaponInfo& weaponInfo) override {
		setData(data);

		auto timeConverter = [](const String& s)->int { return std::stoi(s) * 1000; };

		bool result = setProperty<String>(&weaponInfo.name, "Name", toString);

		result &= setProperty<int>(&weaponInfo.maxAmmo, "MaxAmmo", toInt);
		result &= setProperty<int>(&weaponInfo.initialAmmo, "InitialAmmo", toInt);
		result &= setProperty<int>(&weaponInfo.packAmmo, "PackAmmo", toInt);
		result &= setProperty<int>(&weaponInfo.seriesLength, "SeriesLength", toInt);
		result &= setProperty<int>(&weaponInfo.missilesNumber, "Missiles", toInt);
		result &= setProperty<int>(&weaponInfo.shotTime, "ShotTime", timeConverter);
		result &= setProperty<int>(&weaponInfo.reloadTime, "ReloadTime", timeConverter);

		result &= setProperty<float>(&weaponInfo.missileSpeed, "MissileSpeed", toFloat);
		result &= setProperty<float>(&weaponInfo.missileLength, "MissileLength", toFloat);
		result &= setProperty<float>(&weaponInfo.minDamage, "MinDamage", toFloat);
		result &= setProperty<float>(&weaponInfo.maxDamage, "MaxDamage", toFloat);
		result &= setProperty<float>(&weaponInfo.damageRadius, "DamageRadius", toFloat);
		result &= setProperty<float>(&weaponInfo.fireAngle, "ShotAngle", [](const String& s)->float {
			return std::stof(s) * common::PI_F / 180.0f; });
		result &= setProperty<float>(&weaponInfo.explosionSpeed, "ExplosionSpeed", toFloat);

		result &= setProperty<bool>(&weaponInfo.stopsAtTarget, "StopsAtTarget", toBool);
		weaponInfo.explodes = weaponInfo.explosionSpeed > 0;

		int r, g, b;
		result &= setProperty<int>(&r, "ColorR", toInt);
		result &= setProperty<int>(&g, "ColorG", toInt);
		result &= setProperty<int>(&b, "ColorB", toInt);
		weaponInfo.color.r = r;
		weaponInfo.color.g = g;
		weaponInfo.color.b = b;

		return result;
	}
};