local level_common = require 'level-common'
local Platfrom = require 'platform'
local Button = require 'button'

local level_02 = {
    name = 'level-02',
    tileMap = 'tilemaps/level-02.tmx',
    music = 'sounds/level-01/level-01.wav'
}

---@param scene sol.Scene
local function createButton(level, scene)
    local button_position_object = scene:getTileMapObjectByName('button')
    if not button_position_object then
        error('There is no button-position object on the tile map')
    end
    Button.new(
        scene,
        level.pixelPontToMeters(button_position_object.position),
        { layer = level_common.layers.BACKGROUND }
    )
end

---@param scene sol.Scene
function level_02:init(scene)
    createButton(self, scene)

    local platfrom01_point = scene:getTileMapObjectByName('platform-01')
    if not platfrom01_point then
        error('Object platform-01 not found')
    end

    local hook = scene:createBody(
        self.pixelPontToMeters({ x = platfrom01_point.position.x, y = platfrom01_point.position.y }),
        { type = sol.BodyType.STATIC }
    )
    local platform = Platfrom.new(
        scene,
        self.pixelPontToMeters(platfrom01_point.position),
        {
            length = 9,
            bodyType = sol.BodyType.DYNAMIC
        }
    )
    local hook_joint = scene:createDistanceJoint({
        bodyA = hook,
        bodyB = platform
    })

    local result = {}
    function result.onSensorBeginContact(contact)
        if contact.sensor.shapeKey == Button.keys.shapes.SENSOR and contact.visitor.shapeKey == Button.keys.shapes.CAP then
            scene:destroyJoint(hook_joint)
        end
    end
    function result.onSensorEndContact(contact)
        if contact.sensor.shapeKey == Button.keys.shapes.SENSOR and contact.visitor.shapeKey == Button.keys.shapes.CAP then
            hook_joint = scene:createDistanceJoint({
                bodyA = hook,
                bodyB = platform
            })
        end
    end
    return result
end

local module = {}
module.__index = module

---@param on_finish function
function module.run(on_finish)
    return level_common.run(level_02, on_finish)
end

return setmetatable({}, module)
