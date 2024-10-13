local resources = require 'resources'

local SCALE_FACTOR = 0.25

local module = {
    keys = {
        shapes = {
            main = 'main',
            bottomSensor = 'bottom-sensor'
        },
        shapeGraphics = {
            idleLeft = 'ilde-left',
            idleRight = 'ilde-right',
            walkLeft = 'walk-left',
            walkRight = 'walk-right',
            jumpLeft = 'jump-left',
            jumpRight = 'jump-right'
        }
    }
}
module.__index = module

---@param graphics_defs table<string, GraphicsPackDefinition>
local function addIdleAnimation(graphics_defs)
    local frame_duration = 80
    local animation_position = {
        x = -(resources.keys.sprites.knight.idle.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.knight.idle.rect.h * SCALE_FACTOR
    }
    ---@type GraphicsPackFrameDefinition[]
    local frames = {}
    for _, sprite_key in ipairs(resources.keys.sprites.knight.idle) do
        table.insert(
            frames,
            {
                duration = frame_duration,
                sprites = {
                    {
                        sprite = resources.getSprite(sprite_key),
                        scaleFactor = { x = SCALE_FACTOR, y = SCALE_FACTOR }
                    }
                }
            }
        )
    end
    graphics_defs[module.keys.shapeGraphics.idleLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.idleRight] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, GraphicsPackDefinition>
local function addWalkAnimations(graphics_defs)
    local frame_duration = 80
    local animation_position = {
        x = -(resources.keys.sprites.knight.walk.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.knight.walk.rect.h * SCALE_FACTOR + 9
    }
    ---@type GraphicsPackFrameDefinition[]
    local frames = {}
    for _, sprite_key in ipairs(resources.keys.sprites.knight.walk) do
        table.insert(
            frames,
            {
                duration = frame_duration,
                sprites = {
                    {
                        sprite = resources.getSprite(sprite_key),
                        scaleFactor = { x = SCALE_FACTOR, y = SCALE_FACTOR }
                    }
                }
            }
        )
    end
    graphics_defs[module.keys.shapeGraphics.walkLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.walkRight] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, GraphicsPackDefinition>
local function addJumpAnimations(graphics_defs)
    local frame_duration = 52
    local animation_position = {
        x = -(resources.keys.sprites.knight.jump.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.knight.jump.rect.h * SCALE_FACTOR + 9
    }
    local frames = {}
    for _, sprite_key in ipairs(resources.keys.sprites.knight.jump) do
        table.insert(
            frames,
            {
                duration = frame_duration,
                sprites = {
                    {
                        sprite = resources.getSprite(sprite_key),
                        scaleFactor = { x = SCALE_FACTOR, y = SCALE_FACTOR }
                    }
                }
            }
        )
    end
    graphics_defs[module.keys.shapeGraphics.jumpLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.jumpRight] = {
        position = animation_position,
        frames = frames
    }
end

---@type BodyDefinition | nil
local definition = nil

---@return BodyDefinition
local function getDefinition()
    if definition then
        return definition
    end
    local hit_box = {
        w = 240 * SCALE_FACTOR,
        h = 525 * SCALE_FACTOR
    }
    hit_box.x = -(hit_box.w / 2)
    hit_box.y = -hit_box.h
    ---@type BodyDefinition
    definition = {
        type = sol.BodyType.DYNAMIC,
        script = 'player-script.lua',
        physics = {
            fixedRotation = true
        },
        shapes = {
            [module.keys.shapes.main] = {
                type = sol.BodyShapeType.POLYGON,
                physics = {
                    friction = 1.1,
                    density = 80
                },
                rect = hit_box,
                graphics = {}
            },
            [module.keys.shapes.bottomSensor] = {
                type = sol.BodyShapeType.POLYGON,
                physics = {
                    isSensor = true
                },
                rect = {
                    x = hit_box.x + 2,
                    y = hit_box.y + hit_box.h,
                    w = hit_box.w - 4,
                    h = 5
                }
            }
        }
    }
    addIdleAnimation(definition.shapes[module.keys.shapes.main].graphics)
    addWalkAnimations(definition.shapes[module.keys.shapes.main].graphics)
    addJumpAnimations(definition.shapes[module.keys.shapes.main].graphics)
    return definition
end

---@param scene sol.Scene
---@param position Point
---@param script_argument any?
---@return sol.Body
function module.new(scene, position, script_argument)
    local body = scene:createBody(position, getDefinition(), script_argument)
    local main_shape = body:getShape(module.keys.shapes.main)
    if main_shape then
        main_shape:setCurrentGraphics(module.keys.shapeGraphics.idleRight)
    else
        error('There is no shape ' .. module.keys.shapes.main .. ' in the player body')
    end
    return body
end

return setmetatable({}, module)
