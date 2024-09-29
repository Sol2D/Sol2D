---@class Level01
---@field scene sol.Scene
---@field store sol.Store

require 'player'

---@class Resources
---@field platformSpriteSheet sol.SpriteSheet

local METERS_PER_PIXEL = 0.01
local ONE_WAY_PLATFORM_SHAPE_KEY = 'platform'

---@param store sol.Store
---@return Resources
local function loadResources(store)
    local resources = {}
    resources.platformSpriteSheet = store:createSpriteSheet(ONE_WAY_PLATFORM_SHAPE_KEY)
    if (not resources.platformSpriteSheet:loadFromFile(
            'sprites/platform/platform.png',
            { colCount = 3, rowCount = 1, spriteWidth = 128, spriteHeight = 64 }
        )) then
        print('Unable to load sprites/platform/platform.png')
    end
    return resources
end

---@param level Level01
---@param resources Resources
local function createPlatform(level, resources)
    local platformGraphics = level.store:createGraphicsPack('platform-01-left')
    local frame = platformGraphics:addFrame()
    platformGraphics:addSprite(frame, resources.platformSpriteSheet, 0, { position = { x = 0, y = 0 } })
    platformGraphics:addSprite(frame, resources.platformSpriteSheet, 1, { position = { x = 128, y = 0 } })
    platformGraphics:addSprite(frame, resources.platformSpriteSheet, 2, { position = { x = 256, y = 0 } })
    local map_object = level.scene:getTileMapObjectByName('platform-01')
    if map_object == nil then
        print('Unable to get the "platform-01" object')
        return nil
    end
    local points = {}
    for _, point in ipairs(map_object.points) do
        table.insert(
            points,
            Level01.pixelPointToPhisical({
                x = map_object.position.x + point.x,
                y = map_object.position.y + point.y
            })
        )
    end
    local PLATFORM_GRAPHICS_KEY = 'main'
    local body = level.scene:createBody(
        points[1],
        {
            type = sol.BodyType.KINEMATIC,
            script = 'platform-script.lua',
            shapes = {
                [ONE_WAY_PLATFORM_SHAPE_KEY] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        isPreSolveEnabled = true,
                        restitution = 0.2
                    },
                    rect = { x = 0, y = 0, w = 384, h = 64 },
                    graphics = {
                        [PLATFORM_GRAPHICS_KEY] = {
                            graphics = platformGraphics
                        }
                    }
                }
            }
        },
        { points = points }
    )
    if (not level.scene:setBodyShapeCurrentGraphic(body, ONE_WAY_PLATFORM_SHAPE_KEY, PLATFORM_GRAPHICS_KEY)) then
        print('Unable to set current graphics to platform-01')
    end
end

---@param contact PreSolveContact
---@return boolean
local function preSolveContact(contact)
    local player_side = nil
    local platform_side = nil
    local sign = 0
    if contact.sideA.shapeKey == Player.getMetadata().shapes.main then
        player_side = contact.sideA
        platform_side = contact.sideB
        sign = 1
    elseif contact.sideB.shapeKey == Player.getMetadata().shapes.main then
        player_side = contact.sideB
        platform_side = contact.sideA
        sign = -1
    end
    if player_side == nil or platform_side == nil or platform_side.shapeKey ~= ONE_WAY_PLATFORM_SHAPE_KEY then
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

---@return Level01
local function createLevel()
    local level = {};
    level.store = sol.stores:createStore('level-01')
    level.scene = level.store:createScene(
        'main',
        {
            gravity = { x = 0, y = 80 },
            metersPerPixel = METERS_PER_PIXEL
        }
    )
    level.scene:loadTileMap('tilemaps/level-01.tmx')
    level.scene:createBodiesFromMapObjects('obstacle')
    level.scene:createBodiesFromMapObjects(
        'one-way-platfrom',
        {
            shapeKey = ONE_WAY_PLATFORM_SHAPE_KEY,
            shapePhysics = {
                isPreSolveEnabled = true
            }
        }
    )
    local resources = loadResources(level.store)
    createPlatform(level, resources)
    level.scene:subscribeToPreSolveContact(preSolveContact)
    return level
end

local level01 = {
    createLevel = createLevel,
    metersPerPixel = METERS_PER_PIXEL
}

level01.__index = level01

---@param point Point
---@return Point
function level01.pixelPointToPhisical(point)
    return { x = point.x * METERS_PER_PIXEL, y = point.y * METERS_PER_PIXEL }
end

Level01 = {}
setmetatable(Level01, level01)
return Level01
