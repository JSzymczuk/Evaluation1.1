#pragma once

#include <map>
#include <vector>
#include "main/Configuration.h"
#include "agents/ObjectInfo.h"

class Team;

class SharedKnowledge {
public:
	SharedKnowledge(Team* team);
	~SharedKnowledge();

	ActorInfo getTeamMember(const String& name) const;
	std::vector<ActorInfo> getTeamMembers() const;
	String getVariable(const String& value) const;
	void setVariable(const String& key, const String& value);

private:
	Team* _team;	
};