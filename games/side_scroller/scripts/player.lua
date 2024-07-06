Player = {}

local proto_metatable = {
    key = 'player',
    graphics = {
        idle_right = 'player_idle_right',
        idle_left = 'player_idle_left',
        walk_right = 'player_walk_right',
        walk_left = 'player_walk_left'
    },
    shapes = {
        main = 'player'
    }
}
proto_metatable.__index = proto_metatable

function Player.getMetadata()
    return setmetatable({}, proto_metatable)
end

---@param larder sol.Larder
---@param shape sol.BodyShapePrototype
---@param scale_factor number
local function createIdleAnimation(larder, shape, scale_factor)
    local sprite_loading_options = {
        rect = { x = 58, y = 42, w = 434, h = 615 }
    }
    local sprite_animation_options = {
        duration = 80
    }
    local animation = larder:createSpriteAnimation('player_idle')
    for i = 1, 10, 1 do
        local sprite = larder:createSprite('player-idle-' .. i)
        if not sprite:loadFromFile('sprites/knight/idle (' .. i .. ').png', sprite_loading_options) then
            print('Cannot load player-idle-' .. i)
        end
        sprite:scale(scale_factor) -- TODO: scale to options in loadFromFile
                                   -- TODO: add the scaleSource function to scale the source once
        animation:addFrameFromSprite(sprite, sprite_animation_options)
    end
    local animation_position = {
        x = -(sprite_loading_options.rect.w * scale_factor) / 2,
        y = -sprite_loading_options.rect.h * scale_factor
    }
    shape:addSpriteAnimation(proto_metatable.graphics.idle_right, animation, {
        position = animation_position
    })
    shape:addSpriteAnimation(proto_metatable.graphics.idle_left, animation, {
        position = animation_position,
        isFlippedHorizontally = true
    })
end

---@param larder sol.Larder
---@param shape sol.BodyShapePrototype
---@param scale_factor number
local function createWalkAnimations(larder, shape, scale_factor)
    local sprite_loading_options = {
        rect = { x = 55, y = 31, w = 444, h = 662 }
    }
    local sprite_animation_options = {
        duration = 80,
        position = {
            x = 0,
            y = -34 * scale_factor
        }
    }
    local animation = larder:createSpriteAnimation('player_walk')
    for i = 1, 10, 1 do
        local sprite = larder:createSprite('player-walk-' .. i)
        if not sprite:loadFromFile('sprites/knight/walk (' .. i .. ').png', sprite_loading_options) then
            print('Cannot load player-walk-' .. i) -- TODO: logger
        end
        sprite:scale(scale_factor) -- TODO: scale to options in loadFromFile
                                   -- TODO: add the scaleSource function to scale the source once
        animation:addFrameFromSprite(sprite, sprite_animation_options)
    end
    local animation_position = {
        x = -(sprite_loading_options.rect.w * scale_factor) / 2,
        y = -sprite_loading_options.rect.h * scale_factor
    }
    shape:addSpriteAnimation(proto_metatable.graphics.walk_right, animation, {
        position = animation_position
    })
    shape:addSpriteAnimation(proto_metatable.graphics.walk_left, animation, {
        position = animation_position,
        isFlippedHorizontally = true
    })
end

---@param larder sol.Larder larder where the prototype will be created
function Player.createPrototype(larder)
    local SCALE_FACTOR = 0.25
    local data = setmetatable({}, proto_metatable)
    data.proto = larder:createBodyPrototype(data.key, sol.BodyType.DYNAMIC)
    data.proto:attachScript('player-script.lua')
    local hit_box = {
        w = 240 * SCALE_FACTOR,
        h = 525 * SCALE_FACTOR
    }
    hit_box.x = -(hit_box.w / 2)
    hit_box.y = -hit_box.h
    local shape = data.proto:createPolygonShape(data.shapes.main, hit_box)
    createIdleAnimation(larder, shape, SCALE_FACTOR)
    createWalkAnimations(larder, shape, SCALE_FACTOR)
    return data
end

return Player
