#include "Actor.h"
#include "Team.h"

unsigned short Team::getNumber() const { return _teamNumber; }

SDL_Color Team::getColor() const { return _color; }

bool Team::hasMember(const String& name) const { return _members.find(name) != _members.end(); }

std::vector<Actor*> Team::getMembers() const {
	std::vector<Actor*> result;
	result.reserve(_members.size());
	for (auto it = _members.begin(); it != _members.end(); ++it) {
		result.push_back(it->second);
	}
	return result;
}

Actor* Team::getMember(const String& name) const {
	auto it = _members.find(name);
	return it != _members.end() ? it->second : nullptr;
}

void Team::addMember(const String& name, Actor* member) {
	if (!hasMember(name)) {
		_members[name] = member;
		member->_team = this;
	}
}
