maxHealth = 0
healthLow = 0
currentTriggerId = -1
isWandering = false
maxArmor = 0
mapWidth = 0
mapHeight = 0
preferredWeapon = nil

function initialize(agent, actorKnowledge, time) 
	maxHealth = agent:getActorMaxHealth()
	healthLow = maxHealth / 2
	healthVeryLow = maxHealth / 4
	maxArmor = agent:getActorMaxArmor()
	mapWidth = agent:getMapWidth()
	mapHeight = agent:getMapHeight()
	
	rand = agent:getRandom(0, 3)
	if rand == 0 then
		preferredWeapon = "RocketLauncher"
	elseif rand == 1 then
		preferredWeapon = "Railgun"
	elseif rand == 2 then
		preferredWeapon = "Chaingun"
	else
		preferredWeapon = "Shotgun"
	end	
end

function update(agent, actorKnowledge, time)

	if not actorKnowledge:canInterruptAction() then 
		return 
	end
	
	health = actorKnowledge:getHealth()	
	armor = actorKnowledge:getArmor()	
	currentAction = actorKnowledge:getCurrentAction()	
	weaponType = actorKnowledge:getWeaponType()
	ammo = actorKnowledge:getAmmo(weaponType)
	
	if weaponType ~= preferredWeapon and actorKnowledge:getAmmo(preferredWeapon) > 0 then 
		agent:selectWeapon(preferredWeapon)
		return
	end
	
	if ammo == 0 then
		if actorKnowledge:getAmmo("RocketLauncher") > 0 then 
			agent:selectWeapon("RocketLauncher")
			io.write("RL");
			return
		elseif actorKnowledge:getAmmo("Railgun") > 0 then 
			agent:selectWeapon("Railgun")
			io.write("RG");
			return
		elseif actorKnowledge:getAmmo("Chaingun") > 0 then 
			agent:selectWeapon("Chaingun")
			io.write("CG");
			return
		elseif actorKnowledge:getAmmo("Shotgun") > 0 then 
			agent:selectWeapon("Shotgun")
			io.write("SG");
			return
		end
	end
	
	foes = actorKnowledge:getSeenFoes()
	n = foes:size()	
	
	if n > 0 then
		if ammo > 0 and actorKnowledge:isLoaded(weaponType) then			
			pos = foes:at(0):getPosition()
			agent:shoot(pos)
			return
		end
	end
	
	triggers = agent:getTriggers()
	n = triggers:size()
	
	if currentTriggerId ~= -1 and not triggers:at(currentTriggerId):isActive() then
		currentTriggerId = -1
	end
	
	if currentTriggerId == -1 then
		for i = 0, n - 1, 1 do
			trigger = triggers:at(i)
			if trigger:isActive() then
				triggerType = trigger:getName()
				
				continue = true
				
				if triggerType == "Medpack" then
					if health < maxHealth then 
						continue = false 
					end
				elseif triggerType == "Armor" then
					if armor < maxArmor then 
						continue = false
					end
				elseif actorKnowledge:getAmmo(triggerType) < agent:getMaxAmmo(triggerType) then
					continue = false
				end
						
				if not continue then				
					currentTriggerId = i
					agent:move(trigger:getPosition())
					return
				end
				
			end
		end
	end
	
	if currentAction ~= 1 then
		agent:move(Vector2(agent:getRandom(0, mapWidth), agent:getRandom(0, mapHeight)))
	end
	
end

