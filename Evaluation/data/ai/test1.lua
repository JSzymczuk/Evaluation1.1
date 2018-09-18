--dumb1.lua
last = 0
interval = 1000

function test1Update(agent, actorKnowledge, time)
    --io.write(counter, "\n")
    --counter = counter + 1
    --agent:getName()
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
   
    --if time - last > interval then
        --last = time
	    --io.write(actorKnowledge:getName(), " updated: ", time, "\n")
	   	   	  
	    --if name == "team1_3" then
	    --    agent:notify("team1_2", 200, "OK")
	    --end
    --end
    if agent:getSharedKnowledge():getVariable(agent:getName()) == "" then
		agent:wander()
		agent:getSharedKnowledge():setVariable(agent:getName(), "1")
	end
   
   
end;

function test1Initialize(agent, actorKnowledge, time)
   --io.write("Hello")
end



function getActionName(action)

    if (action == 5) then
        return "Waiting";
    elseif (action == 4) then
        return "Reloading";
    elseif (action == 3) then
        return "Dying";
    elseif (action == 2) then
        return "ChangingWeapon";
    elseif (action == 1) then
        return "Shooting";
    elseif (action == 0) then
        return "Moving";
    end
    return "Unknown";
end;

function log( name, msg)
    io.write( name )
    io.write( ": ")
    io.write( msg)
    io.write( "\n")
end;

function showVector( vector)
    io.write( "(")
    io.write( vector:value(0))
    io.write( ",")
    io.write( vector:value(1))
    io.write( ",")
    io.write( vector:value(2))
    io.write( ",")
    io.write( vector:value(3))
    io.write( ");")
end;