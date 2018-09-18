#pragma once

#include <map>
#include <vector>
#include <mutex>
#include "main/Configuration.h"

class Actor;
struct SDL_Color;

class Team {
public:
	Team(unsigned short number);

	void setColor(const SDL_Color& color);
	SDL_Color getColor() const;
	unsigned short getNumber() const;
	std::vector<Actor*> getMembers() const;
	bool hasMember(const String& name) const;
	Actor* getMember(const String& name) const;
	void addMember(const String& name, Actor* member);
	size_t getSize() const;
	size_t getRemainingActors() const;
	float getTotalRemainingHelath() const;
	void setVariable(const String& key, const String& value, GameTime time);
	String getVariable(const String& key) const;

private:
	unsigned short _teamNumber;
	std::map<String, Actor*> _members;
	std::map<String, std::pair<String, GameTime>> _variables;
	SDL_Color _color;
	mutable std::mutex _mtx;
};
