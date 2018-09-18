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
#include "engine/CommonFunctions.h"
#include "entities/Team.h"
#include "agents/SharedKnowledge.h"

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

void Agent::recieveNotification(Actor* sender, int code, const String& message, GameTime time) {
	_notifications.push_back(Notification(sender, code, message, time));
}

void Agent::addNotificationSender(NotificationSender* sender) {
	_notificationSenders.push_back(sender);
}

std::vector<NotificationSender*> Agent::getNotificationSenders() const {
	return _notificationSenders;
}

std::vector<NotificationListener*> Agent::getNotificationListeners() const {
	return _notificationListeners;
}

void Agent::notify(const String& name, int code, const String& message) {
	GameTime time = Game::getInstance()->getTime();
	for (auto listener : _notificationListeners) {
		if (listener->isRecievingNotifications() && listener->getName() == name) {
			listener->recieveNotification(_actor, code, message, time);
		}
	}
}

void Agent::notifyAll(int code, const String& message) {
	GameTime time = Game::getInstance()->getTime();
	for (auto listener : _notificationListeners) {
		if (listener->isRecievingNotifications()) {
			listener->recieveNotification(_actor, code, message, time);
		}
	}
}

std::vector<Notification> Agent::getNotifications() const {
	int n = _notifications.size() - 1;
	int N = Config.MaxNotifications;
	std::vector<Notification> result;

	for (int i = 0; i < N && n >= 0; ++i, --n) {
		result.push_back(_notifications.at(n));
	}

	return result;
}

SharedKnowledge Agent::getSharedKnowledge() const { 
	return SharedKnowledge(_actor->getTeam()); 
}

String Agent::getName() const { return _actor->getName(); }

bool Agent::isRecievingNotifications() const { return !_actor->isDead(); }

void Agent::addNotificationListener(NotificationListener* listener) { 
	_notificationListeners.push_back(listener);
}

void Agent::removeNotificationListener(NotificationListener* listener) { 
	size_t idx = common::indexOf(_notificationListeners, listener);
	if (idx != _notificationListeners.size()) {
		common::swapLastAndRemove(_notificationListeners, idx);
	}
}

void Agent::removeNotificationSender(NotificationSender* sender) {
	size_t idx = common::indexOf(_notificationSenders, sender);
	if (idx != _notificationSenders.size()) {
		common::swapLastAndRemove(_notificationSenders, idx);
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

std::map<String, std::mutex*> luaMtx;

void LuaAgent::initialize(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luaMtx.at(_initializeScriptName)->lock();
		luabind::call_function<void>(_luaEnv, _initializeScriptName.c_str(), this, actorKnowledge, time);
		luaMtx.at(_initializeScriptName)->unlock();
	}
	catch (luabind::error& e) {
		luaMtx.at(_initializeScriptName)->unlock();
		std::cerr << e.what() << std::endl;
		char n;
		std::cin >> n;
	}
}

void LuaAgent::update(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luaMtx.at(_updateScriptName)->lock();
		luabind::call_function<void>(_luaEnv, _updateScriptName.c_str(), this, actorKnowledge, time);
		luaMtx.at(_updateScriptName)->unlock();
	}
	catch (luabind::error& e) {
		luaMtx.at(_updateScriptName)->unlock();
		std::cerr << e.what() << std::endl;
		char n;
		std::cin >> n;
	}
}

String trimFileName(const String& filename) {
	size_t n = filename.length();
	int slashPos = n - 1;
	int dotPos = n - 1;
	while (dotPos >= 0 && filename.at(dotPos) != '.') { --dotPos; }
	while (slashPos >= 0 && filename.at(slashPos) != '/') { --slashPos; }
	if (dotPos >= 0 && slashPos >= 0 && dotPos - slashPos > 1) {
		return filename.substr(slashPos + 1, dotPos - slashPos - 1);
	}
	std::cerr << "Script name '" << filename << "' is invalid.\n";
}

LuaAgent::LuaAgent(Actor* actor, String filename, LuaEnv* luaEnv) : Agent(actor) {
	int error = luaL_loadfile(luaEnv, filename.c_str()) || lua_pcall(luaEnv, 0, LUA_MULTRET, 0);
	if (error) {
		std::cerr << "[Lua] Error " << error << ": " << lua_tostring(luaEnv, -1) << " - during execution of script: " << filename << "\n";
		lua_pop(luaEnv, 1);
	}
	this->_luaEnv = luaEnv;
	String scriptName = trimFileName(filename);
	_initializeScriptName = (scriptName + Config.LuaInitializeFunctionName);
	_updateScriptName = (scriptName + Config.LuaUpdateFunctionName);
	luaMtx.emplace(_initializeScriptName, new std::mutex());
	luaMtx.emplace(_updateScriptName, new std::mutex());
}