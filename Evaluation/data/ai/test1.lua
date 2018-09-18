--last = 0
--interval = 1000

function test1Update(agent, actorKnowledge, time)
	io.write("test\n")
    --name = agent:getName()
	
	--if agent:getSharedKnowledge():getVariable(agent:getName()) == "" then
	--	agent:wander()
	--	agent:getSharedKnowledge():setVariable(agent:getName(), "1")
	--end
	
    --io.write(counter, "\n")
    --counter = counter + 1
	--actorKnowledge:getTeam()
	--actorKnowledge:getPosition()
	--actorKnowledge:isMoving()
	--goal = agent:getShortDestination()
	--io.write(name, ": ", goal.x, ":", goal.y, "\n")
	
	--io.write(actorKnowledge:getEstimatedRemainingDistance(), "\n")
	
	--agent:getSharedKnowledge():setVariable("test", tostring(time))
	--io.write(time, " ", agent:getSharedKnowledge():getVariable("test"), "\n")
   
    --if name == "team1_2" then
		--sharedKnowledge = agent:getSharedKnowledge()
		--members = sharedKnowledge:getTeamMembers()
		--n = members:size()
	    --io.write(n, "\n")
		
		--for i = 0, n - 1, 1 do
		--	pos = members:at(i):getPosition()
		--	io.write(members:at(i):getName(), ": ", pos.x, ":", pos.y, "\n")
		--end
		
	    --notifications = agent:getNotifications()
	    --n = notifications:size()
	    --for i = 0, n - 1, 1 do
		--    notification = notifications:at(i)
		--    io.write(name, " to ", notification:getSender():getName(), ": ", notification:getMessage(), "\n")
	    --end
	--end
	--if name == "team1_2" then
		--seenActors = actorKnowledge:getSeenActors()
		--n = seenActors:size()		
		--io.write("\nActors:\n")
		--for i = 0, n - 1, 1 do
		--	pos = seenActors:at(i):getPosition()
		--	io.write(seenActors:at(i):getName(), ": ", pos.x, ":", pos.y, "\n")
		--end
		
	    --seenTriggers = actorKnowledge:getSeenTriggers()
		--m = seenTriggers:size()		
		--io.write("\nTriggers:\n");
		--for i = 0, m - 1, 1 do
		--	pos = seenTriggers:at(i):getPosition()
		--	io.write(seenTriggers:at(i):getName(), ": ", pos.x, ":", pos.y, "\n")
		--end
	--end
   
    --if time - last > interval then
        --last = time
	    --io.write(actorKnowledge:getName(), " updated: ", time, "\n")
	   	   	  
	    --if name == "team1_3" then
	    --    agent:notify("team1_2", 200, "OK")
	    --end
    --end
   
end

function test1Initialize(agent, actorKnowledge, time)

end
