local resources = require 'resources'

local SCALE_FACTOR = 0.25

local module = {
    keys = {
        shapes = {
            MAIN = 'main',
            BOTTOM_SENSOR = 'bottom-sensor'
        },
        shapeGraphics = {
            IDLE_LEFT = 'ilde-left',
            IDLE_RIGHT = 'ilde-right',
            WALK_LEFT = 'walk-left',
            WALK_RIGHT = 'walk-right',
            JUMP_LEFT = 'jump-left',
            JUMP_RIGHT = 'jump-right',
            ATTACK_LEFT = 'attac-left',
            ATTACK_RIGHT = 'attac-right'
        }
    }
}
module.__index = module

---@param graphics_defs table<string, sol.GraphicsPackDefinition>
local function addIdleAnimation(graphics_defs)
    local frame_duration = 80
    local animation_position = {
        x = -(resources.keys.sprites.knight.idle.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.knight.idle.rect.h * SCALE_FACTOR
    }
    ---@type sol.GraphicsPackFrameDefinition[]
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
    graphics_defs[module.keys.shapeGraphics.IDLE_LEFT] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.IDLE_RIGHT] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, sol.GraphicsPackDefinition>
local function addWalkAnimations(graphics_defs)
    local frame_duration = 80
    local animation_position = {
        x = -(resources.keys.sprites.knight.walk.rect.w * SCALE_FACTOR) / 2,
        y = -resources.keys.sprites.knight.walk.rect.h * SCALE_FACTOR + 9
    }
    ---@type sol.GraphicsPackFrameDefinition[]
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
    graphics_defs[module.keys.shapeGraphics.WALK_LEFT] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.WALK_RIGHT] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, sol.GraphicsPackDefinition>
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
    graphics_defs[module.keys.shapeGraphics.JUMP_LEFT] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.JUMP_RIGHT] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, sol.GraphicsPackDefinition>
local function addAttackAnimations(graphics_defs)
    local frame_duration = 60
    ---@type sol.GraphicsPackFrameDefinition[]
    local frames = {}
    for _, sprite_key in ipairs(resources.keys.sprites.knight.attack) do
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
    graphics_defs[module.keys.shapeGraphics.ATTACK_LEFT] = {
        isFlippedHorizontally = true,
        position = {
            x = -(resources.keys.sprites.knight.attack.rect.w * SCALE_FACTOR) / 2 - 4,
            y = -resources.keys.sprites.knight.attack.rect.h * SCALE_FACTOR + 1
        },
        frames = frames
    }
    graphics_defs[module.keys.shapeGraphics.ATTACK_RIGHT] = {
        position = {
            x = -(resources.keys.sprites.knight.attack.rect.w * SCALE_FACTOR) / 2 + 4,
            y = -resources.keys.sprites.knight.attack.rect.h * SCALE_FACTOR + 1
        },
        frames = frames
    }
end

---@return sol.BodyDefinition
local function getDefinition()
    local hit_box = {
        w = 240 * SCALE_FACTOR,
        h = 525 * SCALE_FACTOR
    }
    hit_box.x = -(hit_box.w / 2)
    hit_box.y = -hit_box.h
    local radius = hit_box.w / 2
    ---@type sol.BodyDefinition
    local definition = {
        type = sol.BodyType.DYNAMIC,
        physics = {
            fixedRotation = true
        },
        shapes = {
            [module.keys.shapes.MAIN] = {
                type = sol.BodyShapeType.CAPSULE,
                physics = {
                    friction = 1.1,
                    density = 80
                },
                radius = radius,
                center1 = { x = hit_box.x + radius , y = hit_box.y + radius },
                center2 = { x = hit_box.x + radius, y = hit_box.y + hit_box.h - radius },
                graphics = {}
            },
            [module.keys.shapes.BOTTOM_SENSOR] = {
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
    addIdleAnimation(definition.shapes[module.keys.shapes.MAIN].graphics)
    addWalkAnimations(definition.shapes[module.keys.shapes.MAIN].graphics)
    addJumpAnimations(definition.shapes[module.keys.shapes.MAIN].graphics)
    addAttackAnimations(definition.shapes[module.keys.shapes.MAIN].graphics)
    return definition
end

---@param scene sol.Scene
---@param position sol.Point
---@param script_argument any?
---@return sol.Body
function module.new(scene, position, script_argument)
    local player = scene:createBody(position, getDefinition(), 'player-script.lua', script_argument)
    local main_shape = player:getShape(module.keys.shapes.MAIN)
    if main_shape then
        main_shape:setCurrentGraphics(module.keys.shapeGraphics.IDLE_RIGHT)
    else
        error('There is no shape ' .. module.keys.shapes.MAIN .. ' in the player body')
    end
    return player
end

return setmetatable({}, module)
