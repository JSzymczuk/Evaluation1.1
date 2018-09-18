#include "agents/Notification.h"


Notification::Notification(Actor* actor, int code, const String& message, GameTime time)
	: _sender(actor, time), _code(code), _message(message) {}

GameTime Notification::getTime() const { return _sender.getObservationTime(); }

int Notification::getCode() const { return _code; }

String Notification::getMessage() const { return _message; }

ActorInfo Notification::getSender() const { return _sender; }
