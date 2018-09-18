#include "agents/SharedKnowledge.h"
#include "engine/Rng.h"
#include "entities/Team.h"
#include "main/Game.h"


SharedKnowledge::SharedKnowledge(Team* team) : _team(team) {}

SharedKnowledge::~SharedKnowledge() {}

ActorInfo SharedKnowledge::getTeamMember(const String& name) const {
	return ActorInfo(_team->getMember(name), Game::getCurrentTime());
}

std::vector<ActorInfo> SharedKnowledge::getTeamMembers() const {
	std::vector<ActorInfo> result;
	GameTime time = Game::getCurrentTime();
	for (Actor* actor : _team->getMembers()) {
		result.push_back(ActorInfo(actor, time));
	}
	return result;
}

String SharedKnowledge::getVariable(const String& value) const { return _team->getVariable(value); }

void SharedKnowledge::setVariable(const String& key, const String& value) { _team->setVariable(key, value, Game::getCurrentTime()); }

int SharedKnowledge::getMapWidth() const { return Config.ActorRotationSpeed; }

int SharedKnowledge::getMapHeight() const { return Config.ActorRotationSpeed; }

int SharedKnowledge::getRandom(int min, int max) const { return Rng::getInteger(min, max); }

float SharedKnowledge::getActorMaxHealth() const { return Config.ActorMaxHealth; }

float SharedKnowledge::getActorRadius() const { return Config.ActorRadius; }

float SharedKnowledge::getTriggerRadius() const { return Config.TriggerRadius; }

float SharedKnowledge::getActorSightRadius() const { return Config.ActorSightRadius; }

float SharedKnowledge::getActorSpeed() const { return Config.ActorSpeed; }

float SharedKnowledge::getActorRotationSpeed() const { return Config.ActorRotationSpeed; }