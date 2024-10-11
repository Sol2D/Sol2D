local keys = require 'resources.keys'

---@param store sol.Store
local function getSpriteSheet(store)
    local sprite_sheet_key = 'platform'
    local sprite_sheet = store:getSpriteSheet(sprite_sheet_key)
    if sprite_sheet then
        return sprite_sheet
    end
    sprite_sheet = store:createSpriteSheet(sprite_sheet_key)
    if (not sprite_sheet:loadFromFile(
            'sprites/platform/platform.png',
            { colCount = 3, rowCount = 1, spriteWidth = 128, spriteHeight = 64 }
        )) then
        error('Unable to load sprites/platform/platform.png')
    end
    return sprite_sheet
end

---@param store sol.Store
---@param length integer
---@param body_type integer
---@param shape_key string
local function createPlatformPrototype(store, length, body_type, shape_key)
    local SPRITE_WIDTH = 128
    local proto_key = keys.bodies.platfrom(length, body_type)
    local sprite_sheet = getSpriteSheet(store)
    local sprites = {
        {
            sprite = { spriteSheet = sprite_sheet, spriteIndex = 0 },
            position = { x = 0, y = 0 }
        }
    }
    for i = 1, length - 2 do
        table.insert(
            sprites,
            {
                sprite = { spriteSheet = sprite_sheet, spriteIndex = 1 },
                position = { x = i * SPRITE_WIDTH, y = 0 }
            }
        )
    end
    table.insert(
        sprites,
        {
            sprite = { spriteSheet = sprite_sheet, spriteIndex = 2 },
            position = { x = (length - 1) * SPRITE_WIDTH, y = 0 }
        }
    )
    return store:createBodyPrototype(
        proto_key,
        {
            type = body_type,
            shapes = {
                [shape_key] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        isPreSolveEnabled = true,
                        restitution = 0.2,
                        density = 100
                    },
                    rect = { x = 0, y = 0, w = length * SPRITE_WIDTH, h = 64 },
                    graphics = {
                        [keys.shapeGraphics.platform.main] = {
                            animationIterations = 0,
                            frames = { { sprites = sprites } }
                        }
                    }
                }
            }
        }
    )
end

---@class PlatformCreationOptions
---@field length integer? default = 3
---@field bodyType integer? default sol.BodyType.STATIC
---@field shapeKey string? default keys.shapes.oneWayPlatfrom.main
---@field scriptArgument string?

---@param store sol.Store
---@param scene sol.Scene
---@param position Point
---@param options PlatformCreationOptions?
---@return sol.Body
local function createPlatform(store, scene, position, options)
    local length
    local body_type
    local shape_key
    local script_argument
    if options then
        if options.length ~= nil then
            length = options.length
        end
        if options.bodyType ~= nil then
            body_type = options.bodyType
        end
        if options.shapeKey then
            shape_key = options.shapeKey
        end
        if options.scriptArgument ~= nil then
            script_argument = options.scriptArgument
        end
    end
    if length == nil then
        length = 3
    elseif length < 2 then
        error('Platform length cannot be less than 2, ' .. length .. ' requested')
    end
    if body_type == nil then
        body_type = sol.BodyType.STATIC
    end
    if shape_key == nil then
        shape_key = keys.shapes.oneWayPlatfrom.main
    end
    local proto = createPlatformPrototype(store, length, body_type, shape_key)
    local body = scene:createBody(position, proto, script_argument)
    local shape = body:getShape(shape_key)
    if shape then
        shape:setCurrentGraphics(keys.shapeGraphics.platform.main)
    end
    return body
end

return {
    createPlatform = createPlatform
}