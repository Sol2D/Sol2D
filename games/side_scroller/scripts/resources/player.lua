local keys = require 'resources.keys'

local SCALE_FACTOR = 0.25

---@param definitions table<string, BodyShapeGraphicsDefinition>
---@param store sol.Store
local function addIdleAnimation(definitions, store)
    local sprite_loading_options = {
        rect = { x = 58, y = 42, w = 434, h = 615 }
    }
    local frame_options = {
        duration = 80
    }
    local animaion = store:createGraphicsPack(keys.graphicPacks.player.idle)
    local frames_count = 10
    animaion:addFrames(frames_count, frame_options)
    for i = 1, frames_count, 1 do
        local sprite = store:createSprite('player-idle-' .. i)
        if not sprite:loadFromFile('sprites/knight/idle (' .. i .. ').png', sprite_loading_options) then
            error('Cannot load player-idle-' .. i)
        end
        sprite:scale(SCALE_FACTOR) -- TODO: scale to options in loadFromFile
        -- TODO: add the scaleSource function to scale the source once
        animaion:addSprite(i - 1, sprite)
    end
    local animation_position = {
        x = -(sprite_loading_options.rect.w * SCALE_FACTOR) / 2,
        y = -sprite_loading_options.rect.h * SCALE_FACTOR
    }
    definitions[keys.shapeGraphics.player.idleRight] = {
        graphics = animaion,
        position = animation_position
    }
    definitions[keys.shapeGraphics.player.idleLeft] = {
        graphics = animaion,
        position = animation_position,
        isFlippedHorizontally = true
    }
end

---@param graphics_defs table<string, BodyShapeGraphicsDefinition>
---@param store sol.Store
local function addWalkAnimations(graphics_defs, store)
    local sprite_loading_options = {
        rect = { x = 55, y = 31, w = 444, h = 662 }
    }
    local frame_options = {
        duration = 80,
        position = {
            x = 0,
            y = -34 * SCALE_FACTOR
        }
    }
    local animaion = store:createGraphicsPack(keys.graphicPacks.player.walk)
    local frames_count = 10
    animaion:addFrames(frames_count, frame_options)
    for i = 1, frames_count, 1 do
        local sprite = store:createSprite('player-walk-' .. i)
        if not sprite:loadFromFile('sprites/knight/walk (' .. i .. ').png', sprite_loading_options) then
            error('Cannot load player-walk-' .. i)
        end
        sprite:scale(SCALE_FACTOR) -- TODO: scale to options in loadFromFile
        -- TODO: add the scaleSource function to scale the source once
        animaion:addSprite(i - 1, sprite, { position = { x = 0, y = 9 } })
    end
    local animation_position = {
        x = -(sprite_loading_options.rect.w * SCALE_FACTOR) / 2,
        y = -sprite_loading_options.rect.h * SCALE_FACTOR
    }
    graphics_defs[keys.shapeGraphics.player.walkRight] = {
        graphics = animaion,
        position = animation_position
    }
    graphics_defs[keys.shapeGraphics.player.walkLeft] = {
        graphics = animaion,
        position = animation_position,
        isFlippedHorizontally = true
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
    store:createBodyPrototype(keys.bodies.player, body_definition)
end

return {
    init = createPlayerPrototype
}