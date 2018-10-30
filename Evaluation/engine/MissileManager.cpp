#include "MissileManager.h"
#include "engine/Navigation.h"
#include "entities/Actor.h"
#include "entities/Missile.h"
#include "engine//WeaponLoader.h"
#include "math/Math.h"
#include "actions//Die.h"

MissileManager::MissileManager() {
	_capacity = DEFAULT_CAPACITY;
	_count = 0;
	_missiles = new Missile[_capacity];
}

MissileManager::~MissileManager() {
	delete[] _missiles;
}

const WeaponInfo& getWeaponInfo(const String& weaponName) { return MissileManager::getWeaponInfo(weaponName); }

Segment getMissileLine(const Missile& missile, const GameTime& time) {
	Vector2 direction = (missile.target - missile.origin).normal();
	auto weaponInfo = getWeaponInfo(missile.weaponType);
	float front = weaponInfo.missileSpeed * (time - missile.timeFired) / 1000000;
	return Segment(missile.origin + direction * front, missile.origin + direction * common::max(front - weaponInfo.missileLength, 0));
}

bool MissileManager::_weaponInfoInitialized = false;
std::map<String, WeaponInfo> MissileManager::_weaponInfo = std::map<String, WeaponInfo>();

void MissileManager::initializeMissile(int missileIndex, MissileOwner* owner, const Vector2& position, const Vector2& target, GameTime time) {
	Missile& missile = _missiles[missileIndex];
	missile.owner = owner;
	missile.origin = position + ((target - position).normal() * Config.MissileInitialDistance);
	missile.target = target;
	missile.timeFired = time;
	missile.weaponType = owner->getCurrentWeapon();
	missile.isActive = true;
	missile.isTargetReached = false;
	missile.frontPosition = missile.origin;
	missile.backPosition = missile.origin;
}

void MissileManager::shootAt(MissileOwner* owner, const Vector2& target, GameTime time) {
	Vector2 pos = owner->getPosition();
	if (common::sqDist(pos, target) > common::sqr(Config.MissileInitialDistance)) {
		const WeaponInfo& weaponInfo = getWeaponInfo(owner->getCurrentWeapon());
		if (weaponInfo.missilesNumber == 1) { initializeMissile(getNextIndex(), owner, pos, target, time); }
		else {
			Vector2 missileTarget = common::rotatePoint(target, pos, -weaponInfo.fireAngle / 2);
			initializeMissile(getNextIndex(), owner, pos, missileTarget, time);
			float angle = weaponInfo.fireAngle / (weaponInfo.missilesNumber - 1);
			float valCos = cosf(angle), valSin = sinf(angle);
			for (int i = 1; i < weaponInfo.missilesNumber; ++i) {
				missileTarget = common::rotatePoint(missileTarget, pos, valCos, valSin);
				initializeMissile(getNextIndex(), owner, pos, missileTarget, time);
			}
		}
	}
}

void MissileManager::invokeDamage(Missile& missile, const Vector2& point) {
	auto weaponInfo = getWeaponInfo(missile.weaponType);
	float radius = weaponInfo.damageRadius;
	std::vector<Destructible*> potentialTargets = _map->checkCollisionDestructible(point, radius);
	for (Destructible* entity : potentialTargets) {			
			float sqDist = entity->getSquareDistanceTo(point);
			float maxSqDist = radius * radius;
			if (sqDist <= maxSqDist) {
				entity->recieveDamage(weaponInfo.minDamage + (1 - sqDist / maxSqDist) * (weaponInfo.maxDamage - weaponInfo.minDamage)); 
				if (entity->wasDestroyed()) {
					missile.owner->registerKill(entity);
					entity->onDestroy();
				}
		}
	}
}

void MissileManager::missileHit(Missile& missile, const Vector2& point, GameTime time) {
	missile.isTargetReached = true;
	missile.timeHit = time;
	invokeDamage(missile, point);
}

void MissileManager::update(GameTime time) {
	for (int i = 0; i < _count; ++i) {
		Missile& missile = _missiles[i];
		if (missile.isActive) {
			Segment missileSegment = getMissileLine(missile, time);
			Vector2 backPositionOld = missile.backPosition;
			missile.backPosition = missileSegment.to;

			if (missile.isTargetReached) {
				//missile.isActive = false;
				if (common::sqDist(missile.origin, missile.frontPosition)
					< common::sqDist(missile.origin, missile.backPosition)) {
					missile.backPosition = missile.frontPosition;
					const WeaponInfo& weaponInfo = getWeaponInfo(missile.weaponType);
					if (!weaponInfo.explodes || time - missile.timeHit > 1000000 * weaponInfo.damageRadius / weaponInfo.explosionSpeed) {
						missile.isActive = false;
					}
				}
			}
			else {
				missile.frontPosition = missileSegment.from;
				Segment extendedSegment(backPositionOld, missile.frontPosition);
				auto possibleColliders = _map->checkCollision(extendedSegment);
				
				for (DynamicEntity* entity : possibleColliders) {
					if (entity->isSolid()) {
						auto collisionResult = common::testCircleAndSegment(common::Circle{ entity->getPosition(), entity->getRadius() }, extendedSegment);
						if (collisionResult.pointsFound == 1) {
							missileHit(missile, collisionResult.first, time);
						}
						else if (collisionResult.pointsFound == 2) {
							missileHit(missile, 
								common::sqDist(backPositionOld, collisionResult.first)
								< common::sqDist(backPositionOld, collisionResult.second)
								? collisionResult.first
								: collisionResult.second, time);
						}
					}
				}

				if (!missile.isTargetReached) {
					Vector2 wallHitPoint;
					if (_map->raycastStatic(Segment(missile.origin, missile.frontPosition), wallHitPoint)) {
						missile.frontPosition = wallHitPoint;
						missileHit(missile, wallHitPoint, time);
					}
					else if (getWeaponInfo(missile.weaponType).stopsAtTarget
						&& (missile.frontPosition - missile.origin).lengthSquared()
						> (missile.target - missile.origin).lengthSquared()) {
						missileHit(missile, missile.target, time);
					}
				}
			}
		}
	}
}

void MissileManager::initialize(GameMap* map) {
	_map = map;
	if (!_weaponInfoInitialized) {
		initializeWeaponInfo();
		_weaponInfoInitialized = true;
	}
}


int MissileManager::getNextIndex() {
	// Najpierw próbujemy przydzieliæ pierwszy dostêpny indeks
	for (int i = 0; i < _count; ++i) {
		if (!_missiles[i].isActive) {
			return i;
		}
	}
	// Jeœli nie jest to mo¿liwe, zwiêkszamy pulê wykorzystywanych indeksów.
	// W ostatecznoœci poszerzamy tablicê.
	if (_count == _capacity) {
		_capacity += RESIZE_CAPACITY;
		Missile* tempArray = new Missile[_capacity];
		memcpy(tempArray, _missiles, sizeof(Missile) * _count);
		delete[] _missiles;
		_missiles = tempArray;
	}
	return _count++;
}

std::vector<Missile> MissileManager::getMissiles() const {
	std::vector<Missile> result;
	result.reserve(_count);
	for (int i = 0; i < _count; ++i) {
		if (_missiles[i].isActive && common::sqDist(_missiles[i].backPosition, _missiles[i].frontPosition) > common::EPSILON) {
			result.push_back(_missiles[i]);
		}
	}
	return result;
}

std::vector<common::Ring> MissileManager::getExplosions(GameTime time) const {
	std::vector<common::Ring> result;
	for (int i = 0; i < _count; ++i) {
		const WeaponInfo& weaponInfo = getWeaponInfo(_missiles[i].weaponType);
		if (_missiles[i].isActive && _missiles[i].isTargetReached) {
			float r = (time - _missiles[i].timeHit) * weaponInfo.explosionSpeed / 1000000 * 3 / 2;
			float r1 = common::max(r - weaponInfo.damageRadius / 2, 0);
			float r2 = common::min(r, weaponInfo.damageRadius);
			result.push_back({ _missiles[i].frontPosition, r1, r2 });
		}
	}
	return result;
}

const WeaponInfo& MissileManager::getWeaponInfo(const String& weaponName) { return _weaponInfo[weaponName]; }

const std::map<String, WeaponInfo>& MissileManager::getWeaponsInfo() { return _weaponInfo; }

void MissileManager::initializeWeaponInfo() {

	std::ifstream infile(Config.WeaponsDataFile);
	std::map<std::string, std::string> lines;
	std::string line, name, property;

	int i = 0;

	while (std::getline(infile, line)) {
		++i;
		if (line.find_first_not_of(' ') != std::string::npos) {
			std::istringstream iss(line);
			if (!(iss >> name >> property) || lines.find(name) != lines.end()) {
				Logger::log(Config.WeaponsDataFile + ": syntax error on line " + std::to_string(i) + ".");
			}
			else { lines[name] = property; }
		}
		else {
			if (lines.size() > 0) {
				WeaponInfo weaponInfo;
				if (WeaponLoader().load(lines, weaponInfo)) {
					_weaponInfo[weaponInfo.name] = weaponInfo;
				}
				lines.clear();
			}
		}
	}

	if (lines.size() > 0) {
		WeaponInfo weaponInfo;
		if (WeaponLoader().load(lines, weaponInfo)) {
			_weaponInfo[weaponInfo.name] = weaponInfo;
		}
	}
	
}
