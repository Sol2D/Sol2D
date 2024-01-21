local function createPlayerPrototype()
    local player_proto = sol.world:createBodyPrototype()
    player_proto:loadSpriteSheet('assets/player/george.png', {
        spriteWidth = 32,
        spriteHeight = 36,
        rowCount = 4,
        colCount = 4,
        marginTop = 8,
        marginLeft = 8,
        horizontalSpacing = 16,
        verticalSpacing = 12
    })
    player_proto:attachScript('player.lua')
    return player_proto;
end

local function setupLevel01(player_proto)
    local teleport_1 = 'Teleport_1'
    local teleport_2 = 'Teleport_2'
    local disabled_teleport = ''
    local level = sol.world:loadLevelFromTmx('tiled/tmx/level-01.tmx')
    if not level then
        print('Unable to load level')
        return
    end
    level:createBox2dObstaclesFromObjects('Obstacle')
    level:createBox2dSensorsFromObjects('Sensor')
    local player = level:createBody(player_proto)
    player:setLayer('Ground')
    level:followBody(player)
    level:onBodyContacted(
        function(body, other) -- FIXME: for a single body. Must be moved to player.lua
            print('Contacted', body, other, other.name)
            if other.name == disabled_teleport then
                return
            elseif other.name == teleport_1 then
                disabled_teleport = teleport_2
                local position = level:getObjectByName(teleport_2)
                body:setPosition(position.position.x, position.position.y)
            elseif other.name == teleport_2 then
                disabled_teleport = teleport_1
                local position = level:getObjectByName(teleport_1)
                body:setPosition(position.position.x, position.position.y)
            elseif other.name == 'SchoolEnter' then
--                level:destroy()
--                TODO: setupLevel02()
            end
        end
    )
    level:onBodyDiscontacted(
        function(body, other) -- FIXME: for a single body. Must be moved to player.lua
            print('Discontacted', body, other, other.name)
            if other.name == disabled_teleport then
                disabled_teleport = ''
            end
        end
    )
end

setupLevel01(createPlayerPrototype())
