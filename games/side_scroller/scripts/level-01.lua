---@class Level01
---@field scene sol.Scene
---@field store sol.Store

require 'player'

---@class Resources
---@field platformSpriteSheet sol.SpriteSheet

local meters_per_pixel = 0.01
local platform_shape_key = 'platform'

---@param level Level01
---@return Resources
local function loadResources(level)
    local resources = {}
    resources.platformSpriteSheet = level.store:createObject('sol.SpriteSheet', platform_shape_key)
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
    local body_proto = level.store:createObject('sol.BodyPrototype', 'platform-01', sol.BodyType.KINEMATIC)
    local map_object = level.scene:getTileMapObjectByName('platform-01')
    if map_object == nil then
        print('Unable to get the "platofm-01" object')
        return nil
    end
    local shape_proto = body_proto:createPolygonShape(
        'platform',
        { x = 0, y = 0, w = 384, h = 64 }
    )
    local graphics = level.store:createObject('sol.GraphicsPack', 'platform-01-left')
    local frame = graphics:addFrame()
    graphics:addSprite(frame, resources.platformSpriteSheet, 0, { position = { x = 0, y = 0 } })
    graphics:addSprite(frame, resources.platformSpriteSheet, 1, { position = { x = 128, y = 0 } })
    graphics:addSprite(frame, resources.platformSpriteSheet, 2, { position = { x = 256, y = 0 } })
    shape_proto:addGraphics('platform', graphics)
    local body = level.scene:createBody(Level01.pixelPointToPhisical(map_object.position), body_proto)
    if(not level.scene:setBodyShapeCurrentGraphic(body, 'platform', 'platform')) then
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
    if player_side == nil or platform_side == nil or platform_side.shapeKey ~= platform_shape_key then
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
    local store = sol.stores:createStore('level-01')
    local scene = store:createObject(
        'sol.Scene',
        'main',
        { gravity = { x = 0, y = 80 }, metersPerPixel = meters_per_pixel }
    )
    scene:loadTileMap('tilemaps/level-01.tmx')
    local level = {
        scene = scene,
        store = store
    }
    local resources = loadResources(level)
    createPlatform(level, resources)
    scene:subscribeToPreSolveContact(preSolveContact)
    return level
end

local level01 = {
    createLevel = createLevel,
    metersPerPixel = meters_per_pixel
}

level01.__index = level01

---@param point Point
---@return Point
function level01.pixelPointToPhisical(point)
    return { x = point.x * meters_per_pixel, y = point.y * meters_per_pixel }
end

Level01 = { }
setmetatable(Level01, level01)
return Level01
