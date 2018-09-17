--dumb1.lua
counter = 0

function test1Update(agent, actorKnowledge, time)
   --io.write(counter, "\n")
   --counter = counter + 1
end;

function test1Initialize(agent, actorKnowledge, time)
   io.write("Hello")
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