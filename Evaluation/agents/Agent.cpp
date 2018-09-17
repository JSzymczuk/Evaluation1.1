#include "agents/Agent.h"
#include "main/Configuration.h"
#include "actions/ChangeWeapon.h"
#include "actions/Move.h"
#include "actions/Shoot.h"
#include "actions/Face.h"
#include "actions/Idle.h"
#include "entities/Actor.h"
#include "agents/ActorKnowledge.h"
#include <iostream>
#include "main/Game.h"


void Agent::start() {
	_hasStarted = true;
	initialize(ActorKnowledge(_actor), Game::getInstance()->getTime());
	_thread = std::thread(&Agent::run, this);
	_actor->setCurrentAction(new IdleAction(_actor));
}

void Agent::run() {
	if (_hasStarted) {
		Actor* actor = getActor();
		auto game = Game::getInstance();

		while (!actor->isDead()) {
			std::unique_lock<std::mutex> lk(game->_updateMutex);
			game->_updateHolder.wait(lk);
			lk.unlock();
			GameTime time = Game::getInstance()->getTime();

			update(ActorKnowledge(_actor), time);
			actor->update(time);
		}

		Game::getInstance()->registerAgentToDispose(this);
	}
}


bool Agent::trySetAction(Action* action) {
	if (!_actor->setCurrentAction(action)) {
		delete action;
		return false;
	}
	return true;
}

Agent::Agent(Actor* actor) : _actor(actor), _hasStarted(false) {}

Actor* Agent::getActor() { return _actor; }
const Actor* Agent::getActor() const { return _actor; }

void Agent::selectWeapon(const String& weaponName) {
	trySetAction(new ChangeWeaponAction(_actor, weaponName));
}
void Agent::move(const Vector2& target) {
	trySetAction(new MoveAction(_actor, target));
}
void Agent::face(const Vector2& target) {
	trySetAction(new FaceAction(_actor, target));
}
void Agent::shoot(const Vector2& target) {
	trySetAction(new ShootAction(_actor, target));
}
void Agent::wait() {
	trySetAction(new IdleAction(_actor));
}
void Agent::moveDirection(const Vector2& direction) {
	trySetAction(new MoveAtAction(_actor, direction));
}
void Agent::wander() {
	trySetAction(new WanderAction(_actor));
}


void LuaAgent::initialize(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luabind::call_function<void>(_luaEnv, (_name + LuaInitializeFunctionName).c_str(), this, actorKnowledge, time);
	}
	catch (luabind::error& e) {
		std::cerr << e.what() << std::endl;
		char n;
		std::cin >> n;
	}
}

void LuaAgent::update(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luabind::call_function<void>(_luaEnv, (_name + LuaUpdateFunctionName).c_str(), this, actorKnowledge, time);
	}
	catch (luabind::error& e) {
		std::cerr << e.what() << std::endl;
		char n;
		std::cin >> n;
	}
}

LuaAgent::LuaAgent(Actor* actor, String filename, LuaEnv* luaEnv) : Agent(actor) {
	int error = luaL_loadfile(luaEnv, filename.c_str()) || lua_pcall(luaEnv, 0, LUA_MULTRET, 0);
	if (error) {
		std::cerr << "[Lua] Error " << error << ": " << lua_tostring(luaEnv, -1) << " - during execution of script: " << filename << "\n";
		lua_pop(luaEnv, 1);
	}
	this->_luaEnv = luaEnv;
	_name = actor->getName();
}