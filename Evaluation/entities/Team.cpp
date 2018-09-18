#include "Actor.h"
#include "Team.h"

Team::Team(unsigned short number) : _teamNumber(number) {}

unsigned short Team::getNumber() const { return _teamNumber; }

size_t Team::getSize() const { return _members.size(); }

SDL_Color Team::getColor() const { return _color; }

void Team::setColor(const SDL_Color& color) { _color = color; }

bool Team::hasMember(const String& name) const { 
	_mtx.lock();
	bool result = _members.find(name) != _members.end();
	_mtx.unlock();
	return result;
}

std::vector<Actor*> Team::getMembers() const {
	std::vector<Actor*> result;
	_mtx.lock();
	result.reserve(_members.size());
	for (auto it = _members.begin(); it != _members.end(); ++it) {
		result.push_back(it->second);
	}
	_mtx.unlock();
	return result;
}

Actor* Team::getMember(const String& name) const {
	_mtx.lock();
	auto it = _members.find(name);
	Actor* value = it != _members.end() ? it->second : nullptr;
	_mtx.unlock();
	return value;
}

void Team::addMember(const String& name, Actor* member) {
	if (!hasMember(name)) {
		_mtx.lock();
		_members[name] = member;
		member->_team = this;
		_mtx.unlock();
	}
}

void Team::setVariable(const String& key, const String& value, GameTime time) {
	_mtx.lock();
	auto it = _variables.find(key);
	if (it == _variables.end() || it->second.second < time) {
		_variables[key] = std::make_pair(value, time);
	}
	_mtx.unlock();
}

String Team::getVariable(const String& key) const {
	_mtx.lock();
	auto it = _variables.find(key);
	String value = it != _variables.end() ? it->second.first : "";
	_mtx.unlock();
	return value;
}

size_t Team::getRemainingActors() const {
	size_t alive = 0;
	for (auto entry : _members) {
		if (!entry.second->isDead()) {
			alive++;
		}
	}
	return alive;
}

 float Team::getTotalRemainingHelath() const {
	float health = 0;
	for (auto entry : _members) {
		health += entry.second->getHealth();
	}
	return health;
}