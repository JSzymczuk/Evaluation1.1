#pragma once

#include "main/Configuration.h"
#include "actions/ChangeWeapon.h"
#include "actions/Move.h"
#include "actions/Shoot.h"
#include "actions/Face.h"
#include "actions/Idle.h"
#include "entities/Actor.h"

class Agent {
public:
	Agent(Actor* actor);
	virtual ~Agent() = default;
	virtual void initialize(GameTime time) = 0;
	virtual void update(GameTime time) = 0;

	Actor* getActor();
	const Actor* getActor() const;

	void selectWeapon(const String& weaponName);
	void move(const Vector2& target);
	void face(const Vector2& target);
	void shoot(const Vector2& target);
	void wait();
	//void moveDirection(const Vector2& target);

private:
	Actor* _actor;
};

Agent::Agent(Actor* actor) : _actor(actor) {}

Actor* Agent::getActor() { return _actor; }
const Actor* Agent::getActor() const { return _actor; }

void Agent::selectWeapon(const String& weaponName) {
	_actor->setCurrentAction(new ChangeWeaponAction(_actor, weaponName));
}
void Agent::move(const Vector2& target) {
	_actor->setCurrentAction(new MoveAction(_actor, target));
}
void Agent::face(const Vector2& target) {
	_actor->setCurrentAction(new FaceAction(_actor, target));
}
void Agent::shoot(const Vector2& target) {
	_actor->setCurrentAction(new ShootAction(_actor, target));
}
void Agent::wait() {
	_actor->setCurrentAction(new IdleAction(_actor));
}
/*void Agent::moveDirection(const Vector2& target) {
current->getActor()->setMoveVector(direction);
}*/

#include "agents/ActorKnowledge.h"

class PlayerAgent : public Agent {
public:
	PlayerAgent(Actor* actor) : Agent(actor) {}
	void initialize(GameTime time);
	void update(GameTime time);
	
protected:
	virtual void initialize(const ActorKnowledge& actorKnowledge, GameTime time) = 0;
	virtual void update(const ActorKnowledge& actorKnowledge, GameTime time) = 0;
	//ActorKnowledge * current;
	//std::unique_ptr<Action> nextAction;
};

void PlayerAgent::update(GameTime time) {
	update(ActorKnowledge(getActor()), time);
}

void PlayerAgent::initialize(GameTime time) {
	initialize(ActorKnowledge(getActor()), time);
}

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

String LuaInitializeFunctionName = "Initialize";
String LuaUpdateFunctionName = "Update";

class LuaAgent : PlayerAgent {

private:
	LuaEnv* _luaEnv;
	String _name;
	//std::string onStartName;

protected:
	void initialize(const ActorKnowledge& actorKnowledge, GameTime time) {
		try {
			luabind::call_function<void>(_luaEnv, (_name + LuaInitializeFunctionName).c_str(), this, actorKnowledge, time);
		}
		catch (luabind::error& e) {
			std::cerr << e.what() << std::endl;
			char n;
			std::cin >> n;
		}
	}
	void update(const ActorKnowledge& actorKnowledge, GameTime time) {
		try {
			luabind::call_function<void>(_luaEnv, (_name + LuaUpdateFunctionName).c_str(), this, actorKnowledge, time);
		}
		catch (luabind::error& e) {
			std::cerr << e.what() << std::endl;
			char n;
			std::cin >> n;
		}
	}

public:
	LuaAgent(Actor* actor, String filename, LuaEnv* luaEnv) : PlayerAgent(actor) {
		int error = luaL_loadfile(luaEnv, filename.c_str()) || lua_pcall(luaEnv, 0, LUA_MULTRET, 0);
		if (error) {
			std::cerr << "[Lua] Error " << error << ": " << lua_tostring(luaEnv, -1) << " - during execution of script: " << filename << "\n";
			lua_pop(luaEnv, 1);
		}
		this->_luaEnv = luaEnv;
		
		int pos = filename.find("/");
		if (pos >= 0) {
			filename = filename.substr(pos + 1);
		}
		_name = filename;
		//name.resize(name.length() - 4);
		//whatToName = std::string(name + "whatTo");
		//onStartName = std::string(name + "onStart");
	}

};
