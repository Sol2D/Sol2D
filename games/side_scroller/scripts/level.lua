local Player = require 'player'
local Platform = require 'platform'

local layers = {
    BACKGROUND = 'background',
    GROUND_1 = 'ground-1',
    GROUND_2 = 'ground-2'
}

local keys = {
    shapes = {
        ONE_WAY_PLATFORM = 'one-way-platform',
        WATER = 'water',
        FINISH = 'finish'
    }
}

--@param game Game
local function createLevel(game)
    local level = {}
    level.__index = level
    level.METERS_PER_PIXEL = 0.01

    ---@param point sol.Point
    ---@return sol.Point
    local function pixelPontToMeters(point)
        return {
            x = point.x * level.METERS_PER_PIXEL,
            y = point.y * level.METERS_PER_PIXEL
        }
    end

    local function getPlayerStartPosition()
        local start_point = level.scene:getTileMapObjectByName('start-position')
        if not start_point then
            error('There is no object named start-position on the fragment map')
        end
        return pixelPontToMeters(start_point.position)
    end

    ---@param contact sol.PreSolveContact
    local function preSolveContact(contact)
        local player_side = nil
        local platform_side = nil
        local sign = 0
        if contact.sideA.bodyId == level.player.id then
            player_side = contact.sideA
            platform_side = contact.sideB
            sign = 1
        elseif contact.sideB.bodyId == level.player.id then
            player_side = contact.sideB
            platform_side = contact.sideA
            sign = -1
        end
        if player_side == nil or platform_side == nil or platform_side.shapeKey ~= keys.shapes.ONE_WAY_PLATFORM then
            return true
        end

        if contact.manifold.normal.y * sign < 0.95 then
            return false;
        end

        for _, point in ipairs(contact.manifold.points) do
            if point.separation > -0.2 then
                return true
            end
        end

        return false
    end

    ---@param on_finish function
    local function createSensorContactListener(on_finish)
        ---@param contact sol.SensorContact
        local function listener(contact)
            if contact.visitor.bodyId == level.player.id then
                if contact.sensor.shapeKey == keys.shapes.WATER then
                    level.player.body:setPosition(getPlayerStartPosition())
                elseif contact.sensor.shapeKey == keys.shapes.FINISH then
                    on_finish()
                end
            end
        end
        return listener
    end

    local function createFlyingPlatforms(scene)
        local platforms = scene:getTileMapObjectsByClass('platform')
        for _, platform in ipairs(platforms) do
            local points = {}
            for _, point in ipairs(platform.points) do
                table.insert(
                    points,
                    pixelPontToMeters({
                        x = platform.position.x + point.x,
                        y = platform.position.y + point.y
                    })
                )
            end
            Platform.new(
                scene,
                points[1],
                {
                    bodyType = sol.BodyType.KINEMATIC,
                    length = 3,
                    layer = layers.GROUND_2,
                    script = 'flying-platform-script.lua',
                    scriptArgument = { points = points }
                }
            )
        end
    end

    ---@param view sol.View
    ---@param on_finish function
    function level:run(view, on_finish)
        ---@type sol.Scene
        level.scene = self:createScene(view)
        level.scene:createBodiesFromMapObjects('obstacle')
        level.scene:createBodiesFromMapObjects('sensor', { shapePhysics = { isSensor = true } })
        level.scene:createBodiesFromMapObjects(
            'one-way-platform',
            {
                shapeKey = keys.shapes.ONE_WAY_PLATFORM,
                shapePhysics = {
                    isPreSolveEnabled = true
                }
            }
        )
        level.scene:createBodiesFromMapObjects(
            'water',
            {
                shapeKey = keys.shapes.WATER,
                shapePhysics = {
                    isSensor = true
                }
            }
        )
        createFlyingPlatforms(level.scene)

        level.player = {
            body = Player.new(
                level.scene,
                getPlayerStartPosition()
            )
        }
        level.player.id = level.player.body:getId()
        level.scene:setFollowedBody(level.player.body)
        level.scene:subscribeToPreSolveContact(preSolveContact)
        level.scene:subscribeToSensorBeginContact(createSensorContactListener(on_finish))

        if self.createMusic then
            local music = self:createMusic()
            if music then
                game.playMusic(music)
            end
        end
    end

    return level
end

return createLevel
