#include "main/Configuration.h"
#include <algorithm>
#include <fstream>

std::map<String, String> loadConfigurationParameters(const String& filename) {
	std::ifstream reader;
	std::map<String, String> parameters;
	reader.open(filename);

	if (!reader.fail()) {
		String key, value;
		while (reader >> key >> value) {
			parameters[key] = value;
		}
		reader.close();
	}

	return parameters;
}

ConfigurationParameters Config(loadConfigurationParameters("config"));

int readAsInt(const String& str) { return std::stoi(str); }
float readAsFloat(const String& str) { return std::stof(str); }
bool readAsBool(const String& str) {
	String t = "true";
	return std::equal(str.begin(), str.end(),
		t.begin(), t.end(),
		[](char a, char b) {
		return tolower(a) == tolower(b);
	});
}

ConfigurationParameters::ConfigurationParameters(const std::map<String, String>& parameters) :
	WindowTitle(parameters.at("WindowTitle")),
	LuaInitializeFunctionName(parameters.at("LuaInitializeFunctionName")),
	LuaUpdateFunctionName(parameters.at("LuaUpdateFunctionName")),
	AgentControlled(parameters.at("AgentControlled")),
	AgentScriptPrefix(parameters.at("AgentScriptPrefix")),
	DefaultWeapon(parameters.at("DefaultWeapon")),
	HealthBarTextureKey(parameters.at("HealthBarTextureKey")),
	HealthBarTexturePath(parameters.at("HealthBarTexturePath")),
	ActorRingTextureKey(parameters.at("ActorRingTextureKey")),
	ActorRingTexturePath(parameters.at("ActorRingTexturePath")),
	TriggerRingTextureKey(parameters.at("TriggerRingTextureKey")),
	TriggerRingTexturePath(parameters.at("TriggerRingTexturePath")),
	MedPackName(parameters.at("MedPackName")),
	ArmorPackName(parameters.at("ArmorPackName")),
	DefaultSettings(parameters.at("DefaultSettings")),
	WeaponsDataFile(parameters.at("WeaponsDataFile")),

	AreLogsVisible(readAsBool(parameters.at("AreLogsVisible"))),
	CollisionMethodAabbTree(readAsBool(parameters.at("CollisionMethodAabbTree"))),

	ActorMaxHealth(readAsInt(parameters.at("ActorMaxHealth"))),
	ActorRadius(readAsInt(parameters.at("ActorRadius"))),
	DisplayWidth(readAsInt(parameters.at("DisplayWidth"))),
	DisplayHeight(readAsInt(parameters.at("DisplayHeight"))),
	FPS(readAsInt(parameters.at("FPS"))),
	RegularGridSize(readAsInt(parameters.at("RegularGridSize"))),
	TriggerRadius(readAsInt(parameters.at("TriggerRadius"))),
	ActorSelectionRing(readAsInt(parameters.at("ActorSelectionRing"))),
	ActorSightRadius(readAsInt(parameters.at("ActorSightRadius"))),
	ActorVOCheckRadius(readAsInt(parameters.at("ActorVOCheckRadius"))),
	ActorVOCheckAngle(readAsInt(parameters.at("ActorVOCheckAngle"))),
	MissileInitialDistance(readAsInt(parameters.at("MissileInitialDistance"))),
	MinInitialTriggerActivationTime(readAsInt(parameters.at("MinInitialTriggerActivationTime"))),
	MinTriggerActivationTime(readAsInt(parameters.at("MinTriggerActivationTime"))),
	MaxTriggerActivationTime(readAsInt(parameters.at("MaxTriggerActivationTime"))),
	TriggerDeactivationTime(readAsInt(parameters.at("TriggerDeactivationTime"))),
	WeaponChangeTime(readAsInt(parameters.at("WeaponChangeTime"))),
	ActorDyingTime(readAsInt(parameters.at("ActorDyingTime"))),
	MaxRecalculations(readAsInt(parameters.at("MaxRecalculations"))),
	HealthBarWidth(readAsInt(parameters.at("HealthBarWidth"))),
	HealthBarHeight(readAsInt(parameters.at("HealthBarHeight"))),
	ArmorMaxShots(readAsInt(parameters.at("ArmorMaxShots"))),
	ArmorTriggerBonus(readAsInt(parameters.at("ArmorTriggerBonus"))),
	ActionPositionHistoryLength(readAsInt(parameters.at("ActionPositionHistoryLength"))),
	ActorUpdateFrequency(readAsInt(parameters.at("ActorUpdateFrequency"))),
	MaxNotifications(readAsInt(parameters.at("MaxNotifications"))),

	VOSideVelocityMargin(readAsFloat(parameters.at("VOSideVelocityMargin"))),
	MaxMovementWaitingTime(readAsFloat(parameters.at("MaxMovementWaitingTime"))),
	MaxRecalculatedWaitingTime(readAsFloat(parameters.at("MaxRecalculatedWaitingTime"))),
	ActorOscilationRadius(readAsFloat(parameters.at("ActorOscilationRadius"))),
	TriggerRotationSpeed(readAsFloat(parameters.at("TriggerRotationSpeed"))),
	ActorRotationSpeed(readAsFloat(parameters.at("ActorRotationSpeed"))),
	ActorSpeed(readAsFloat(parameters.at("ActorSpeed"))),
	MovementGoalMargin(readAsFloat(parameters.at("MovementGoalMargin"))),
	CollisionTreeMovablePadding(readAsFloat(parameters.at("CollisionTreeMovablePadding"))),
	MovementSafetyMargin(readAsFloat(parameters.at("MovementSafetyMargin"))),
	ActorWanderSpread(readAsFloat(parameters.at("ActorWanderSpread"))),
	WanderCentripetalForce(readAsFloat(parameters.at("WanderCentripetalForce"))),
	ActorInitialHealth(readAsFloat(parameters.at("ActorInitialHealth"))),
	ArmorTriggerMultiplier(readAsFloat(parameters.at("ArmorTriggerMultiplier"))),
	MedpackHealthBonus(readAsFloat(parameters.at("MedpackHealthBonus"))),
	MaxArmor(readAsFloat(parameters.at("MaxArmor")))
{}