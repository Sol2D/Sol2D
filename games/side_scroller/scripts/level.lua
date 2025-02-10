local Player = require 'player'

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

    ---@param view sol.View
    ---@param fragmet_id integer
    function level:run(view, fragmet_id)
        ---@type sol.Scene
        level.scene = self:createScene()
        level.scene:createBodiesFromMapObjects('obstacle')
        level.scene:createBodiesFromMapObjects('sensor', { shapePhysics = { isSensor = true } })
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
