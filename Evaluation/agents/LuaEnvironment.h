#pragma once

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <iostream>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>

#include "math/Vector2.h"
#include "main/Configuration.h"
#include "agents/Agent.h"
#include "agents/ActorKnowledge.h"
#include "agents/Notification.h"
#include "agents/SharedKnowledge.h"

typedef lua_State LuaEnv;

typedef std::vector<ActorInfo>::reference(std::vector<ActorInfo>::*ActorInfoReference)(std::vector<ActorInfo>::size_type);
typedef std::vector<TriggerInfo>::reference(std::vector<TriggerInfo>::*TriggerInfoReference)(std::vector<TriggerInfo>::size_type);
typedef std::vector<Notification>::reference(std::vector<Notification>::*NotificationReference)(std::vector<Notification>::size_type);

inline LuaEnv* createLuaEnv() {
	LuaEnv * luaEnv = luaL_newstate();
	luaL_openlibs(luaEnv);
	luabind::open(luaEnv);
	luabind::module(luaEnv)[
		luabind::class_<Vector2>("Vector2")
			.property("x", &Vector2::getX, &Vector2::setX)
			.property("y", &Vector2::getY, &Vector2::setY)
			.def(luabind::constructor<>())
			.def(luabind::constructor<float, float>())
			.def(luabind::constructor<const Vector2>())
			.def("get", &Vector2::get)
			.def("set", &Vector2::set)
			.def("dot", &Vector2::dot)
			.def("length", &Vector2::length)
			.def("lengthSquared", &Vector2::lengthSquared)
			.def("normal", &Vector2::normal)
			.def("normalize", &Vector2::normalize)
			.def(luabind::self * luabind::other<float>())
			.def(luabind::self / luabind::other<float>())
			.def(luabind::self + luabind::other<Vector2>())
			.def(luabind::self - luabind::other<Vector2>())
			.def(luabind::self == luabind::other<Vector2>()),

		luabind::class_<ActorInfo>("ActorInfo")
			.def("getName", &ActorInfo::getName)
			.def("getTeam", &ActorInfo::getTeam)
			.def("getPosition", &ActorInfo::getPosition)
			.def("getOrientation", &ActorInfo::getOrientation)
			.def("getHealth", &ActorInfo::getHealth)
			.def("getArmor", &ActorInfo::getArmor)
			.def("getWeaponType", &ActorInfo::getWeaponType)
			.def("isDead", &ActorInfo::isDead)
			.def("getObservationTime", &TriggerInfo::getObservationTime),

		luabind::class_<ActorKnowledge>("ActorKnowledge")
			.def("getSelf", &ActorKnowledge::getSelf)
			.def("getName", &ActorKnowledge::getName)
			.def("getTeam", &ActorKnowledge::getTeam)
			.def("getPosition", &ActorKnowledge::getPosition)
			.def("isMoving", &ActorKnowledge::isMoving)
			.def("isDead", &ActorInfo::isDead)
			.def("hasPositionChanged", &ActorKnowledge::hasPositionChanged)
			.def("getAmmo", &ActorKnowledge::getAmmo)
			.def("getOrientation", &ActorKnowledge::getOrientation)
			.def("getHealth", &ActorKnowledge::getHealth)
			.def("getArmor", &ActorKnowledge::getArmor)
			.def("getWeaponType", &ActorKnowledge::getWeaponType)
			.def("isLoaded", &ActorKnowledge::isLoaded)
			.def("getSeenFriends", &ActorKnowledge::getSeenFriends)
			.def("getSeenFoes", &ActorKnowledge::getSeenFoes)
			.def("getSeenActors", &ActorKnowledge::getSeenActors)
			.def("getVelocity", &ActorKnowledge::getVelocity)
			.def("getShortDestination", &ActorKnowledge::getShortDestination)
			.def("getLongDestination", &ActorKnowledge::getLongDestination)
			.def("getEstimatedRemainingDistance", &ActorKnowledge::getEstimatedRemainingDistance),

		luabind::class_<TriggerInfo>("TriggerInfo")
			.def("getName", &TriggerInfo::getName)
			.def("getPosition", &TriggerInfo::getPosition)
			.def("isActive", &TriggerInfo::isActive)
			.def("getObservationTime", &TriggerInfo::getObservationTime),

		luabind::class_<Notification>("Notification")
			.def("getCode", &Notification::getCode)
			.def("getMessage", &Notification::getMessage)
			.def("getTime", &Notification::getTime)
			.def("getSender", &Notification::getSender),

		luabind::class_<std::vector<Notification>>("Notifications")
			.def("size", &std::vector<Notification>::size)
			.def("at", (NotificationReference)&std::vector<Notification>::at),

		luabind::class_<std::vector<ActorInfo>>("VectorOfActorInfo")
			.def("size", &std::vector<ActorInfo>::size)
			.def("at", (ActorInfoReference)&std::vector<ActorInfo>::at),

		luabind::class_<std::vector<TriggerInfo>>("VectorOfTriggerInfo")
			.def("size", &std::vector<TriggerInfo>::size)
			.def("at", (TriggerInfoReference)&std::vector<TriggerInfo>::at),

		luabind::class_<std::vector<int>>("VectorOfInt")
			.def("size", &std::vector<int>::size)
			.def("at", (int&(std::vector<int>::*)(size_t))&std::vector<int>::at),

		luabind::class_<SharedKnowledge>("SharedKnowledge")
			.def("getTeamMember", &SharedKnowledge::getTeamMember)
			.def("getTeamMembers", &SharedKnowledge::getTeamMembers)
			.def("getVariable", &SharedKnowledge::getVariable)
			.def("setVariable", &SharedKnowledge::setVariable)
			.def("getMapWidth", &SharedKnowledge::getMapWidth)
			.def("getMapHeight", &SharedKnowledge::getMapHeight)
			.def("getRandom", &SharedKnowledge::getRandom)
			.def("getActorMaxHealth", &SharedKnowledge::getActorMaxHealth)
			.def("getActorRadius", &SharedKnowledge::getActorRadius)
			.def("getTriggerRadius", &SharedKnowledge::getTriggerRadius)
			.def("getActorSightRadius", &SharedKnowledge::getActorSightRadius)
			.def("getActorSpeed", &SharedKnowledge::getActorSpeed)
			.def("getActorRotationSpeed", &SharedKnowledge::getActorRotationSpeed),

		luabind::class_<LuaAgent>("LuaAgent")
			.def("getName", &LuaAgent::getName)
			.def("move", &LuaAgent::move)
			.def("face", &LuaAgent::face)
			.def("wait", &LuaAgent::wait)
			.def("moveDirection", &LuaAgent::moveDirection)
			.def("selectWeapon", &LuaAgent::selectWeapon)
			.def("shoot", &LuaAgent::shoot)
			.def("wander", &LuaAgent::wander)
			.def("notify", &LuaAgent::notify)
			.def("notifyAll", &LuaAgent::notifyAll)
			.def("getNotifications", &LuaAgent::getNotifications)
			.def("getSharedKnowledge", &LuaAgent::getSharedKnowledge)

	];
	return luaEnv;
}

inline static void destroyLuaEnv(LuaEnv* luaEnv) {
	lua_close(luaEnv);
}
