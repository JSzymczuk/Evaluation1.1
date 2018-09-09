#pragma once

#include "SDL.h"
#include <string>

typedef std::string String;
typedef unsigned long long int GameTime;

const int DisplayWidth = 960;
const int DisplayHeight = 720;
const int FPS = 30;

const int TriggerRadius = 20;
const int ActorRadius = 15;
const int ActorSelectionRing = 24;
const int ActorSightRadius = 300;
const int ActorVOCheckRadius = 80;
const int ActorVOCheckAngle = 30;
const float VOSideVelocityMargin = 0.30f;
const int MissileInitialDistance = 30;
const int MinInitialTriggerActivationTime = 0;
const int MinTriggerActivationTime = 5000000;
const int MaxTriggerActivationTime = 15000000;
const int TriggerDeactivationTime = 15000000;
const int WeaponChangeTime = 1200000;
const int ActorDyingTime = 3000000;
const float MaxMovementWaitingTime = 500000;
const float MaxRecalculatedWaitingTime = 1000;
const int MaxRecalculations = 5;
const float TriggerRotationSpeed = 0.05f;
const float ActorRotationSpeed = 0.25f;
const float ActorSpeed = 3.1f;
const float MovementGoalMargin = 2.0f;
const float CollisionTreeMovablePadding = 1.5f;
const float MovementSafetyMargin = 4.0f;
const float AgentWanderSpread = 0.20f;
const float WanderCentripetalForce = 25000;
const float ActorInitialHealth = 0.70f;
const int ActorMaxHealth = 100;
const int HealthBarWidth = 80;
const int HealthBarHeight = 5;
const SDL_Color HealthBarBackColor = { 63, 63, 63 };
const SDL_Color HealthBarFullColor = { 0, 215, 80 };
const SDL_Color HealthBarHalfColor = { 215, 215, 0 };
const SDL_Color HealthBarEmptyColor = { 215, 0, 0 };
const std::string HealthBarTextureKey = "health_bar";
const std::string HealthBarTexturePath = "content/health_bar.png";
const std::string ActorRingTextureKey = "actor_ring";
const std::string ActorRingTexturePath = "content/actor_ring.png";
const std::string TriggerRingTextureKey = "trigger_ring";
const std::string TriggerRingTexturePath = "content/trigger_ring.png";

const String MapsDirectory = "data/maps/";

const String WeaponsDataFile = "data/weapons.dat";
const String DefaultWeapon = "Shotgun";

const int ARMOR_MAX_SHOTS = 5;
const int ARMOR_TRIGGER_BONUS = 10;
const float ARMOR_TRIGGER_MULTIPLIER = 0.5f;
const float MEDPACK_HEALTH_BONUS = 15;
const float MAX_ARMOR = 50;

namespace colors {
	const SDL_Color white = { 255, 255, 255, 255 };
	const SDL_Color black = { 0, 0, 0, 255 };
	const SDL_Color gray = { 127, 127, 127, 255 };
	const SDL_Color darkGray = { 63, 63, 63, 63 };
	const SDL_Color red = { 255, 63, 63, 255 };
	const SDL_Color green = { 63, 255, 63, 255 };
	const SDL_Color blue = { 63, 63, 255, 255 };
	const SDL_Color yellow = { 255, 255, 63, 255 };
	const SDL_Color pink = { 255, 63, 255, 255 };
	const SDL_Color cyan = { 63, 255, 255, 255 };
	const SDL_Color darkRed = { 127, 63, 63, 255 };
	const SDL_Color darkBlue = { 63, 63, 127, 255 };
}

