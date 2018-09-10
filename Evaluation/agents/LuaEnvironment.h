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

typedef lua_State LuaEnv;

LuaEnv* createLuaEnv() {
	LuaEnv * luaEnv = luaL_newstate();
	luaL_openlibs(luaEnv);
	luabind::open(luaEnv);
	luabind::module(luaEnv)[
		luabind::class_<LuaAgent>("LuaAgent")
			.def("test", &LuaAgent::test),
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
			.def(luabind::self == luabind::other<Vector2>())
	];
	return luaEnv;
}

static void destroyLuaEnv(LuaEnv* luaEnv) {
	lua_close(luaEnv);
}
