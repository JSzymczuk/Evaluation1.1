initWandering = false
triggerTarget = nil
isWandering = false

function initialize(agent, actorKnowledge, time) end

function update(agent, actorKnowledge, time)

	--io.write(agent:getName(), ": ", actorKnowledge:getCurrentAction(), "\n")
		
	-- Znajdź najdalszego widzianego sojusznika
	-- i wyślij mu powiadomienie
	
	seenActors = actorKnowledge:getSeenActors()
	n = seenActors:size()
	pos = actorKnowledge:getPosition()
	furthestAlly = -1
	furthestAllyDistance = 0
		
	for i = 0, n - 1, 1 do	
		otherPos = seenActors:at(i):getPosition()
		vecToOther = otherPos - pos
		distance = vecToOther.x * vecToOther.x + vecToOther.y * vecToOther.y
		if distance > furthestAllyDistance then
			furthestAllyDistance = distance
			furthestAlly = i
		end
	end	
	
	if furthestAlly ~= -1 then
		--agent:notify(seenActors:at(furthestAlly):getName(), 200, "OK")
	end

	-- Uaktualnij informację o nadawcy ostatnio
	-- otrzymanego powiadomienia w wiedzy współdzielonej
	
	sharedKnowledge = agent:getSharedKnowledge()
	notifications = agent:getNotifications()
	n = notifications:size()
	
	if n > 0 then
		notification = notifications:at(0)
		sharedKnowledge():setVariable(agent:getName(), notification:getSender():getName())
	end 			
	
	-- Sprawdź czy szukany wyzwalacz jest aktywny,
	-- jeśli nie znajdź nowy i idź do niego	
	
	seenTriggers = actorKnowledge:getSeenTriggers()
	n = seenTriggers:size()
		
	if n > 0 then
		if triggerTarget ~= nil then
			triggerFound = false
			i = 0
			while i < n and not triggerFound do
				trigger = seenTriggers:at(i)
				if triggerName == triggerTarget then
					triggerFound = true
				end
				i = i + 1
			end
			if not triggerFound then
				triggerTarget = nil
				initWandering = true
			end
		end
		if triggerTarget == nil then
			triggerTargetId = sharedKnowledge:getRandom(0, n - 1)
			triggerTarget = seenTriggers:at(triggerTargetId):getName()
			agent:move(seenTriggers:at(triggerTargetId):getPosition())
			initWandering = false
			isWandering = false
		end
	else 
		triggerTarget = nil		
		if not isWandering then
			initWandering = true
		end
	end	
	
	if initWandering then
		agent:wander()
		initWandering = false
		isWandering = true
	end
	
end

