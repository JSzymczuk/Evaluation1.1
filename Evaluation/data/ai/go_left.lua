isMoving = false

function update(agent, actorKnowledge, time)
	if not isMoving then 
		agent:move(Vector2(280, 360))
		isMoving = true
	end
end

function initialize(agent, actorKnowledge, time) end
