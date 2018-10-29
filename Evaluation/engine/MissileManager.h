#pragma once

#include <map>
#include <vector>
#include "entities/Missile.h"
#include "entities/Weapon.h"
#include "math/Math.h"

class GameMap;
struct Ring;

class MissileManager {
public:
	MissileManager();
	~MissileManager();

	void initialize(GameMap* map);
	void update(GameTime time);
	void shootAt(MissileOwner* owner, const Vector2& target, GameTime time);
	std::vector<Missile> getMissiles() const;
	std::vector<common::Ring> getExplosions(GameTime time) const;

	static const WeaponInfo& getWeaponInfo(const String& weaponName);
	static const std::map<String, WeaponInfo>& getWeaponsInfo();

private:
	GameMap* _map;
	static std::map<String, WeaponInfo> _weaponInfo;
	static bool _weaponInfoInitialized;

	void initializeWeaponInfo();
	void invokeDamage(Missile& missile, const Vector2& point);

	Missile* _missiles;
	int _capacity;
	int _count;

	static const int DEFAULT_CAPACITY = 32;
	static const int RESIZE_CAPACITY = 16;

	void initializeMissile(int missileIndex, MissileOwner* owner, const Vector2& position, const Vector2& target, GameTime time);
	void missileHit(Missile& missile, const Vector2& point, GameTime time);
	int getNextIndex();
};

Segment getMissileLine(const Missile& missile, const GameTime& time);

const WeaponInfo& getWeaponInfo(const String& weaponName);