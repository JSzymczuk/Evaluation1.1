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
#include "agents/Notification.h"

class Action;
class Actor;
class ActorKnowledge;
class SharedKnowledge;
class Vector2;
class Game;
class Notification;
typedef lua_State LuaEnv;


class Agent : public NotificationListener, public NotificationSender {
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

	std::vector<Notification> getNotifications() const;
	SharedKnowledge getSharedKnowledge() const;

	void addNotificationListener(NotificationListener* listener) override;
	void removeNotificationListener(NotificationListener* listener) override;
	std::vector<NotificationListener*> getNotificationListeners() const override;
	void notify(const String& name, int code, const String& message) override;
	void notifyAll(int code, const String& message) override;

	String getName() const override;
	bool isRecievingNotifications() const override;
	void addNotificationSender(NotificationSender* sender) override;
	void removeNotificationSender(NotificationSender* sender) override;
	std::vector<NotificationSender*> getNotificationSenders() const override;
	void recieveNotification(Actor* sender, int code, const String& message, GameTime time) override;

protected:
	virtual void initialize(const ActorKnowledge& actorKnowledge, GameTime time) = 0;
	virtual void update(const ActorKnowledge& actorKnowledge, GameTime time) = 0;

private:
	Actor* _actor;
	std::thread _thread;
	bool _hasStarted;

	std::vector<NotificationSender*> _notificationSenders;
	std::vector<NotificationListener*> _notificationListeners;

	std::vector<Notification> _notifications;
	std::vector<ObjectInfo> _seenObjects;

	bool trySetAction(Action* action);

	friend class Game;
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
	String _initializeScriptName;
	String _updateScriptName;
};
