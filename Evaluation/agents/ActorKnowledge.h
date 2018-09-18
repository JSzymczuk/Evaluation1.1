#pragma once

class ActorInfo;
class TriggerInfo;
class Notification;

class ActorKnowledge {
public:
	ActorKnowledge(Actor* actor);
	~ActorKnowledge();

	ActorInfo getSelf() const;
	String getName() const;
	unsigned short getTeam() const;
	Vector2 getPosition() const;
	Vector2 getDestination() const;
	bool isMoving() const;
	float getOrientation() const;
	int getHealth() const;
	int getArmor() const;
	String getWeaponType() const;
	int getAmmo(const String& weaponName) const;
	bool isLoaded(const String& weaponName) const;
	std::vector<ActorInfo> getSeenFriends() const;
	std::vector<ActorInfo> getSeenFoes() const;
	std::vector<ActorInfo> getSeenActors() const;
	std::vector<TriggerInfo> getSeenTriggers() const;

private:
	Actor* _actor;
};