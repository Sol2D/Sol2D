local keys = require 'resources.keys'

local SCALE_FACTOR = 0.25

---@param graphics_defs table<string, GraphicsPackDefinition>
---@param store sol.Store
local function addIdleAnimation(graphics_defs, store)
    local sprite_loading_options = {
        rect = { x = 58, y = 42, w = 434, h = 615 }
    }
    local frame_duration = 80
    local animation_position = {
        x = -(sprite_loading_options.rect.w * SCALE_FACTOR) / 2,
        y = -sprite_loading_options.rect.h * SCALE_FACTOR
    }
    ---@type GraphicsPackFrameDefinition[]
    local frames = {}
    local frames_count = 10
    for i = 1, frames_count, 1 do
        local sprite = store:createSprite('idle-player-' .. i)
        if not sprite:loadFromFile('sprites/knight/idle (' .. i .. ').png', sprite_loading_options) then
            error('Cannot load idle-player-' .. i)
        end
        sprite:scale(SCALE_FACTOR) -- TODO: scale to options in loadFromFile
        ---@type GraphicsPackFrameDefinition
        local frame = {
            duration = frame_duration,
            sprites = { { sprite = sprite } }
        }
        table.insert(frames, frame)
    end
    graphics_defs[keys.shapeGraphics.player.idleLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[keys.shapeGraphics.player.idleRight] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, GraphicsPackDefinition>
---@param store sol.Store
local function addWalkAnimations(graphics_defs, store)
    local sprite_loading_options = {
        rect = { x = 55, y = 31, w = 444, h = 662 }
    }
    local frame_duration = 80
    local animation_position = {
        x = -(sprite_loading_options.rect.w * SCALE_FACTOR) / 2,
        y = -sprite_loading_options.rect.h * SCALE_FACTOR + 9
    }
    local frames = {}
    local frames_count = 10
    for i = 1, frames_count, 1 do
        local sprite = store:createSprite('walk-player-' .. i)
        if not sprite:loadFromFile('sprites/knight/walk (' .. i .. ').png', sprite_loading_options) then
            error('Cannot load walk-player-' .. i)
        end
        sprite:scale(SCALE_FACTOR) -- TODO: scale to options in loadFromFile
        ---@type GraphicsPackFrameDefinition
        local frame = {
            duration = frame_duration,
            sprites = { { sprite = sprite } }
        }
        table.insert(frames, frame)
    end
    graphics_defs[keys.shapeGraphics.player.walkLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[keys.shapeGraphics.player.walkRight] = {
        position = animation_position,
        frames = frames
    }
end

---@param graphics_defs table<string, GraphicsPackDefinition>
---@param store sol.Store
local function addJumpAnimations(graphics_defs, store)
    local sprite_loading_options = {
        rect = { x = 23, y = 27, w = 480, h = 668 }
    }
    local frame_duration = 52
    local animation_position = {
        x = -(sprite_loading_options.rect.w * SCALE_FACTOR) / 2,
        y = -sprite_loading_options.rect.h * SCALE_FACTOR + 9
    }
    local frames = {}
    local frames_count = 10
    for i = 1, frames_count, 1 do
        local sprite = store:createSprite('jump-player-' .. i)
        if not sprite:loadFromFile('sprites/knight/jump (' .. i .. ').png', sprite_loading_options) then
            error('Cannot load jump-player-' .. i)
        end
        sprite:scale(SCALE_FACTOR) -- TODO: scale to options in loadFromFile
        ---@type GraphicsPackFrameDefinition
        local frame = {
            duration = frame_duration,
            sprites = { { sprite = sprite } }
        }
        table.insert(frames, frame)
    end
    graphics_defs[keys.shapeGraphics.player.jumpLeft] = {
        isFlippedHorizontally = true,
        position = animation_position,
        frames = frames
    }
    graphics_defs[keys.shapeGraphics.player.jumpRight] = {
        position = animation_position,
        frames = frames
    }
end

---@param store sol.Store
local function createPlayerPrototype(store)
    local hit_box = {
        w = 240 * SCALE_FACTOR,
        h = 525 * SCALE_FACTOR
    }
    hit_box.x = -(hit_box.w / 2)
    hit_box.y = -hit_box.h
    ---@type BodyDefinition
    local body_definition = {
        type = sol.BodyType.DYNAMIC,
        script = 'bodies/player.lua',
        physics = {
            fixedRotation = true
        },
        shapes = {
            [keys.shapes.player.main] = {
                type = sol.BodyShapeType.POLYGON,
                physics = {
                    friction = 1.1,
                    density = 80
                },
                rect = hit_box,
                graphics = {}
            },
            [keys.shapes.player.bottomSensor] = {
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
    addIdleAnimation(body_definition.shapes[keys.shapes.player.main].graphics, store)
    addWalkAnimations(body_definition.shapes[keys.shapes.player.main].graphics, store)
    addJumpAnimations(body_definition.shapes[keys.shapes.player.main].graphics, store)
    store:createBodyPrototype(keys.bodies.player, body_definition)
end

return {
    init = createPlayerPrototype
}
