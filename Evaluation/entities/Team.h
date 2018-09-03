#pragma once

#include <map>
#include <vector>
#include "main/Configuration.h"

class Actor;
struct SDL_Color;

class Team {
public:
	SDL_Color getColor() const;
	unsigned short getNumber() const;
	std::vector<Actor*> getMembers() const;
	bool hasMember(const String& name) const;
	Actor* getMember(const String& name) const;
	void addMember(const String& name, Actor* member);

private:
	unsigned short _teamNumber;
	std::map<String, Actor*> _members;
	SDL_Color _color;
};