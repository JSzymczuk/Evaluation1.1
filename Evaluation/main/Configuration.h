#pragma once

#include "SDL.h"
#include <string>
#include <map>

typedef std::string String;
typedef unsigned long long int GameTime;

struct ConfigurationParameters {

	const String WindowTitle;
	const int DisplayWidth;
	const int DisplayHeight;
	const int CameraSpeed;
	const int FPS;
	const bool AreLogsVisible;
	const bool MultithreadingEnabled;
	const long long ActorUpdateFrequency;
	const int RegularGridSize;
	const float AabbTreeMargin;
	const String LuaInitializeFunctionName;
	const String LuaUpdateFunctionName;
	const String AgentControlled;
	const String AgentScriptPrefix;

	const int TriggerRadius;
	const int ActorMaxHealth;
	const int ActorRadius;
	const int ActorSelectionRing;
	const int ActorSightRadius;
	const int ActorVOCheckRadius;
	const int ActorVOCheckAngle;
	const float VOSideVelocityMargin;
	const int MissileInitialDistance;
	const int MinInitialTriggerActivationTime;
	const int MinTriggerActivationTime;
	const int MaxTriggerActivationTime;
	const int TriggerDeactivationTime;
	const int WeaponChangeTime;
	const int ActorDyingTime;
	const float MaxMovementWaitingTime;
	const float MaxRecalculatedWaitingTime;
	const int MaxRecalculations;
	const size_t ActionPositionHistoryLength;
	const size_t MaxNotifications;
	const float ActorOscilationRadius;
	const float TriggerRotationSpeed;
	const float ActorRotationSpeed;
	const float ActorSpeed;
	const float MovementGoalMargin;
	const float CollisionTreeMovablePadding;
	const float MovementSafetyMargin;
	const float ActorWanderSpread;
	const float WanderCentripetalForce;
	const float ActorInitialHealth;
	const String DefaultWeapon;
	const int ArmorMaxShots;
	const int ArmorTriggerBonus;
	const float ArmorTriggerMultiplier;
	const float MedpackHealthBonus;
	const float MaxArmor;
	const int HealthBarWidth;
	const int HealthBarHeight;
	const int HealthBarPosition;
	const int ActorNamePosition;
	const int TimerPosition;
	const String WeaponsDataFile;
	const String DefaultSettings;
	const String MedPackName;
	const String ArmorPackName;
	const String ActorRingTextureKey;
	const String ActorRingTexturePath;
	const String TriggerRingTextureKey;
	const String TriggerRingTexturePath;
	const String CollisionResolver;

	const SDL_Color HealthBarBackColor = { 63, 63, 63 };
	const SDL_Color HealthBarFullColor = { 0, 215, 80 };
	const SDL_Color HealthBarHalfColor = { 215, 215, 0 };
	const SDL_Color HealthBarEmptyColor = { 215, 0, 0 };

	ConfigurationParameters(const std::map<String, String>& parameters);
};

extern ConfigurationParameters Config;

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
