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
        WATER = 'water'
    }
}

local function preSolveContact(contact)
    local player_side = nil
    local platform_side = nil
    local sign = 0
    if contact.sideA.shapeKey == Player.keys.shapes.MAIN then
        player_side = contact.sideA
        platform_side = contact.sideB
        sign = 1
    elseif contact.sideB.shapeKey == Player.keys.shapes.MAIN then
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

local function createLevel()
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

    local function getPlayerStartPosition(scene)
        local start_point = scene:getTileMapObjectByName('start-position')
        if not start_point then
            error('There is no object named start-position on the fragment map')
        end
        return pixelPontToMeters(start_point.position)
    end

    local function createFlyingPlatforms(scene)
        local platfroms = scene:getTileMapObjectsByClass('platform')
        for _, platfrom in ipairs(platfroms) do
            local points = {}
            for _, point in ipairs(platfrom.points) do
                table.insert(
                    points,
                    pixelPontToMeters({
                        x = platfrom.position.x + point.x,
                        y = platfrom.position.y + point.y
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
    ---@param fragmet_id integer
    function level:run(view, fragmet_id)
        ---@type sol.Scene
        level.scene = self:createScene()
        level.scene:createBodiesFromMapObjects('obstacle')
        level.scene:createBodiesFromMapObjects('sensor', { shapePhysics = { isSensor = true } })
        level.scene:createBodiesFromMapObjects(
            'one-way-platfrom',
            {
                shapeKey = keys.shapes.ONE_WAY_PLATFORM,
                shapePhysics = {
                    isPreSolveEnabled = true
                }
            }
        )
        createFlyingPlatforms(level.scene)

        local pre_solve_subscription = level.scene:subscribeToPreSolveContact(preSolveContact) -- TODO: unsubscribe

        view:bindFragment(fragmet_id, level.scene)
        level.player = Player.new(
            level.scene,
            getPlayerStartPosition(level.scene)
        )
        level.scene:setFollowedBody(level.player)
    end

    return level
end

return createLevel
