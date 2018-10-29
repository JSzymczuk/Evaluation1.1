#pragma once

#include "main/Configuration.h"
struct SDL_Color;

enum WeaponLoadState {
	WEAPON_LOADED,
	WEAPON_SHOOTING,
	WEAPON_UNLOADED
};

struct WeaponState {
	int ammo;
	GameTime lastShot;
	WeaponLoadState state;
};

struct WeaponInfo {
	String name;
	int maxAmmo;
	int initialAmmo;
	int packAmmo;
	int seriesLength;
	int missilesNumber;
	float missileSpeed;
	float missileLength;
	float minDamage;
	float maxDamage;
	float damageRadius;
	float fireAngle;
	int shotTime;
	int reloadTime;
	bool stopsAtTarget;
	bool explodes;
	float explosionSpeed;
	SDL_Color color;
};
