#pragma once

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

#include <thread>
#include <luabind/luabind.hpp>
#include <luabind/operator.hpp>
#include "main/Configuration.h"

class Action;
class Actor;
class ActorKnowledge;
class Vector2;
typedef lua_State LuaEnv;


class Agent {
public:
	Agent(Actor* actor);
	virtual ~Agent() = default;

	Actor* getActor();
	const Actor* getActor() const;

	void start();
	void run();

	void selectWeapon(const String& weaponName);
	void move(const Vector2& target);
	void face(const Vector2& target);
	void shoot(const Vector2& target);
	void moveDirection(const Vector2& direction);
	void wander();
	void wait();

protected:
	virtual void initialize(const ActorKnowledge& actorKnowledge, GameTime time) = 0;
	virtual void update(const ActorKnowledge& actorKnowledge, GameTime time) = 0;

private:
	Actor* _actor;
	std::thread _thread;
	bool _hasStarted;

	bool trySetAction(Action* action);
};


class PlayerAgent : public Agent {
public:
	PlayerAgent(Actor* actor) : Agent(actor) {}
	
protected:
	void initialize(const ActorKnowledge& actorKnowledge, GameTime time) override {}
	void update(const ActorKnowledge& actorKnowledge, GameTime time) override {}
};


class LuaAgent : public Agent {
public:
	LuaAgent(Actor* actor, String filename, LuaEnv* luaEnv);

protected:
	void initialize(const ActorKnowledge& actorKnowledge, GameTime time) override;
	void update(const ActorKnowledge& actorKnowledge, GameTime time) override;

private:
	LuaEnv* _luaEnv;
	String _name;
};
