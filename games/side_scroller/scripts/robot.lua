local resources = require 'resources'

local SCALE_FACTOR = 0.25

local Robot = {
    keys = {
        shapes = {
            MAIN = 'main',
            -- BOTTOM_SENSOR = 'bottom-sensor'
        },
        shapeGraphics = {
            IDLE_LEFT = 'idle-left',
            IDLE_RIGHT = 'idle-right',
            -- WALK_LEFT = 'walk-left',
            -- WALK_RIGHT = 'walk-right',
            -- JUMP_LEFT = 'jump-left',
            -- JUMP_RIGHT = 'jump-right',
            -- ATTACK_LEFT = 'attack-left',
            -- ATTACK_RIGHT = 'attack-right'
        }
    }
}
Robot.__index = Robot

---@param graphics_defs table<string, sol.GraphicsPackDefinition>
local function addIdleAnimation(graphics_defs)
    local animation_position = {
        x = -(resources.keys.sprites.robot.idle.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.robot.idle.rect.h * SCALE_FACTOR
    }
    local sprite_sheet = resources.getSpriteSheet(resources.keys.spriteSheets.ROBOT_IDLE)
    local frame_duration = 80
    ---@type sol.GraphicsPackFrameDefinition[]
    local frames = {}
    for i = 0, 9 do
        ---@type sol.GraphicsPackFrameDefinition
        local frame = {
            duration = frame_duration,
            sprites = {
                {
                    sprite = {
                        spriteSheet = sprite_sheet,
                        spriteIndex = i
                    },
                    scaleFactor = { x = SCALE_FACTOR, y = SCALE_FACTOR }
                }
            }
        }
        table.insert(frames, frame)
    end
    graphics_defs[Robot.keys.shapeGraphics.IDLE_RIGHT] = {
        frames = frames,
        position = animation_position
    }
    graphics_defs[Robot.keys.shapeGraphics.IDLE_LEFT] = {
        frames = frames,
        position = animation_position,
        isFlippedHorizontally = true
    }
end

---@param scene sol.Scene
---@param position sol.Point
---@param script_argument any?
---@return sol.Body
function Robot.new(scene, position, script_argument)
    local hit_box = {
        w = 275 * SCALE_FACTOR,
        h = 480 * SCALE_FACTOR
    }
    hit_box.x = -(hit_box.w / 2)
    hit_box.y = -hit_box.h
    local radius = hit_box.w / 2
    ---@type sol.BodyShapeCapsuleDefinition
    local main_shape_defenition = {
        type = sol.BodyShapeType.CAPSULE,
        radius = radius,
        center1 = { x = hit_box.x + radius, y = hit_box.y + radius },
        center2 = { x = hit_box.x + radius, y = hit_box.y + hit_box.h - radius },
        graphics = {}
    }
    ---@type sol.BodyDefinition
    local body_definition = {
        physics = {
            position = position,
            type = sol.BodyType.DYNAMIC
        },
        shapes = {
            [Robot.keys.shapes.MAIN] = main_shape_defenition
        }
    }
    addIdleAnimation(main_shape_defenition.graphics)
    local robot = scene:createBody(body_definition)




    local main_shape = robot:getShape(Robot.keys.shapes.MAIN)
    if main_shape then
        main_shape:setCurrentGraphics(Robot.keys.shapeGraphics.IDLE_RIGHT)
    else
        error('There is no shape ' .. Robot.keys.shapes.MAIN .. ' in the robot body')
    end

    return robot

end

return setmetatable({}, Robot)
