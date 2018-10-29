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

void Agent::initialize(GameTime time) {
	initializeLogic(ActorKnowledge(_actor), time);
	_actor->setCurrentAction(new IdleAction(_actor));
	_totalFrames = 0;
}

void Agent::update(GameTime time) {
	Actor* actor = getActor();
	if (!actor->isDead()) {
		updateLogic(ActorKnowledge(_actor), time);
		actor->update(time);
		++_totalFrames;
	}
}

void Agent::run(GameTime time) {
	_hasStarted = true;
	initializeLogic(ActorKnowledge(_actor), time);
	_thread = std::thread(&Agent::runFunc, this);
	_actor->setCurrentAction(new IdleAction(_actor));
	_totalFrames = 0;
}

void Agent::runFunc() {
	if (_hasStarted) {
		Actor* actor = getActor();
		auto game = Game::getInstance();

		while (!actor->isDead()) {
			std::unique_lock<std::mutex> lk(game->_updateMutex);
			game->_updateHolder.wait(lk);
			lk.unlock();
			GameTime time = game->getTime();	

			updateLogic(ActorKnowledge(_actor), time);
			actor->update(time);

			++_totalFrames;
		}

		game->registerAgentToDispose(this);
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

size_t Agent::getTotalFrames() const {
	return _totalFrames;
}


void LuaAgent::initializeLogic(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luabind::call_function<void>(_luaEnv, Config.LuaInitializeFunctionName.c_str(), this, actorKnowledge, time); 
	}
	catch (luabind::error& e) {
		std::cerr << e.what() << std::endl;
		char n;
		std::cin >> n;
	}
}

void LuaAgent::updateLogic(const ActorKnowledge& actorKnowledge, GameTime time) {
	try {
		luabind::call_function<void>(_luaEnv, Config.LuaUpdateFunctionName.c_str(), this, actorKnowledge, time);
	}
	catch (luabind::error& e) {
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
	luaEnv = createLuaEnv();
	this->_luaEnv = luaEnv;
	int error = luaL_loadfile(luaEnv, filename.c_str()) || lua_pcall(luaEnv, 0, LUA_MULTRET, 0);
	if (error) {
		std::cerr << "[Lua] Error " << error << ": " << lua_tostring(luaEnv, -1) << " - during execution of script: " << filename << "\n";
		lua_pop(luaEnv, 1);
	}
}
