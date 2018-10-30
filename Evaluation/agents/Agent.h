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
#include "entities/Entity.h"

class Action;
class Actor;
class ActorKnowledge;
class SharedKnowledge;
class Vector2;
class Game;
class Notification;
typedef lua_State LuaEnv;


class Agent : public virtual NotificationListener, 
	public virtual NotificationSender, 
	public virtual Updatable {
public:
	Agent(Actor* actor);
	virtual ~Agent() = default;

	Actor* getActor();
	const Actor* getActor() const;

	// dla agenta w œrodowisku 1-w¹tkowym
	void initialize(GameTime time);
	void update(GameTime time);

	// dla agenta w œrodowisku wielow¹tkowym
	void run(GameTime time);

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
	   
	int getMapWidth() const;
	int getMapHeight() const;
	int getRandom(int min, int max) const;
	float getActorMaxHealth() const;
	float getActorMaxArmor() const;
	float getMaxAmmo(const String& weaponName) const;
	float getActorRadius() const;
	float getTriggerRadius() const;
	float getActorSightRadius() const;
	float getActorSpeed() const;
	float getActorRotationSpeed() const;
	std::vector<TriggerInfo> getTriggers() const; 
	Vector2 raycastStatic(const Vector2& origin, const Vector2& direction, float length);
	bool checkCircleAndSegment(const Vector2& center, float radius, const Vector2& origin, const Vector2& end);

	size_t getTotalFrames() const;

protected:
	virtual void initializeLogic(const ActorKnowledge& actorKnowledge, GameTime time) = 0;
	virtual void updateLogic(const ActorKnowledge& actorKnowledge, GameTime time) = 0;

private:
	void runFunc();

	Actor* _actor;
	std::thread _thread;
	bool _hasStarted;
	size_t _totalFrames;

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
	void initializeLogic(const ActorKnowledge& actorKnowledge, GameTime time) override {}
	void updateLogic(const ActorKnowledge& actorKnowledge, GameTime time) override {}
};


class LuaAgent : public Agent {
public:
	LuaAgent(Actor* actor, String filename);

protected:
	void initializeLogic(const ActorKnowledge& actorKnowledge, GameTime time) override;
	void updateLogic(const ActorKnowledge& actorKnowledge, GameTime time) override;

private:
	LuaEnv* _luaEnv;
};
