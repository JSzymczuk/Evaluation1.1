isWandering = false

function update(agent, actorKnowledge, time)
	if not isWandering then
		agent:wander()
		isWandering = true
	end
end

function initialize(agent, actorKnowledge, time)
	io.write(agent:getName(), "\n")
end
