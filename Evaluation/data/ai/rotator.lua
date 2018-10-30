x = 0
y = 0

function update(agent, actorKnowledge, time)	
	current = actorKnowledge:getCurrentAction()
	if current == 0 then 
		agent:face(Vector2(agent:getRandom(x - 100, x + 100), agent:getRandom(y - 100, y + 100)))
	end
end

function initialize(agent, actorKnowledge, time) 
	v = actorKnowledge:getPosition()
	x = v.x
	y = v.y
end
