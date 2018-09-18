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

typedef lua_State LuaEnv;

typedef std::vector<ActorInfo>::reference(std::vector<ActorInfo>::*ActorInfoReference)(std::vector<ActorInfo>::size_type);
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

		luabind::class_<LuaAgent>("LuaAgent")
			.def("move", &LuaAgent::move)
			.def("face", &LuaAgent::face)
			.def("wait", &LuaAgent::wait)
			.def("moveDirection", &LuaAgent::moveDirection)
			.def("selectWeapon", &LuaAgent::selectWeapon)
			.def("shoot", &LuaAgent::shoot)
			.def("wander", &LuaAgent::wander)
			.def("notify", &LuaAgent::notify)
			.def("notifyAll", &LuaAgent::notifyAll)
			.def("getNotifications", &LuaAgent::getNotifications),

		luabind::class_<ActorKnowledge>("ActorKnowledge")
			.def("getSelf", &ActorKnowledge::getSelf)
			.def("getName", &ActorKnowledge::getName)
			.def("getTeam", &ActorKnowledge::getTeam)
			.def("getPosition", &ActorKnowledge::getPosition)
			.def("getDestination", &ActorKnowledge::getDestination)
			.def("isMoving", &ActorKnowledge::isMoving)
			.def("getOrientation", &ActorKnowledge::getOrientation)
			.def("getHealth", &ActorKnowledge::getHealth)
			.def("getSeenFriends", &ActorKnowledge::getSeenFriends)
			.def("getSeenFoes", &ActorKnowledge::getSeenFoes)
			.def("getSeenActors", &ActorKnowledge::getSeenActors)
			.def("getArmor", &ActorKnowledge::getArmor)
			.def("getWeaponType", &ActorKnowledge::getWeaponType)
			.def("isLoaded", &ActorKnowledge::isLoaded)
			.def("getAmmo", &ActorKnowledge::getAmmo),

		luabind::class_<ActorInfo>("ActorInfo")
			.def("getName", &ActorInfo::getName)
			.def("getTeam", &ActorInfo::getTeam)
			.def("getPosition", &ActorInfo::getPosition)
			.def("getOrientation", &ActorInfo::getOrientation)
			.def("getHealth", &ActorInfo::getHealth)
			.def("getArmor", &ActorInfo::getArmor)
			.def("getWeaponType", &ActorInfo::getWeaponType),

		luabind::class_<Notification>("Notification")
			.def("getCode", &Notification::getCode)
			.def("getMessage", &Notification::getMessage)
			.def("getTime", &Notification::getTime)
			.def("getSender", &Notification::getSender),

		/*luabind::class_<Enumerations>("Enumerations")
			.enum_("ActionType")
			[
				value("Moving", 0),
				value("Shooting", 1),
				value("ChangingWeapon", 2),
				value("Dying", 3),
				value("Reloading", 4),
				value("Waiting", 5)
			]
		.enum_("WeaponType")
			[
				value("Chaingun", 0),
				value("Railgun", 1),
				value("RocketLuncher", 2),
				value("Shotgun", 3),
				value("WeaponSize", 4)
			]
		,*/

		luabind::class_<std::vector<Notification>>("Notifications")
			.def("size", &std::vector<Notification>::size)
			.def("at", (NotificationReference)&std::vector<Notification>::at),

		luabind::class_<std::vector<ActorInfo>>("vectorOfActorInfo")
			.def("size", &std::vector<ActorInfo>::size)
			.def("at", (ActorInfoReference)&std::vector<ActorInfo>::at),

		luabind::class_<std::vector<int>>("vectorOfInt")
			.def("size", &std::vector<int>::size)
			.def("at", (int&(std::vector<int>::*)(size_t))&std::vector<int>::at)
	];
	return luaEnv;
}

inline static void destroyLuaEnv(LuaEnv* luaEnv) {
	lua_close(luaEnv);
}
