#pragma once

#include "main/Configuration.h"
#include "agents/ActorInfo.h"
#include <vector>


class NotificationSender;
class NotificationListener;


class Notification {
public:
	Notification(Actor* actor, int code, const String& message, GameTime time);

	GameTime getTime() const;
	int getCode() const;
	String getMessage() const;
	ActorInfo getSender() const;

private:
	int _code;
	String _message;
	ActorInfo _sender;
};


class NotificationListener {
public:
	virtual String getName() const = 0;
	virtual bool isRecievingNotifications() const = 0;
	virtual void addNotificationSender(NotificationSender* sender) = 0;
	virtual void removeNotificationSender(NotificationSender* sender) = 0;
	virtual std::vector<NotificationSender*> getNotificationSenders() const = 0;
	virtual void recieveNotification(Actor* sender, int code, const String& message, GameTime time) = 0;
};


class NotificationSender {
public:
	virtual void addNotificationListener(NotificationListener* listener) = 0;
	virtual void removeNotificationListener(NotificationListener* listener) = 0;
	virtual std::vector<NotificationListener*> getNotificationListeners() const = 0;
	virtual void notify(const String& name, int code, const String& message) = 0;
	virtual void notifyAll(int code, const String& message) = 0;
};
