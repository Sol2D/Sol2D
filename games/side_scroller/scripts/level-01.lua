---@class Level01
---@field scene sol.Scene
---@field store sol.Store

---@class Resources
---@field platformSpriteSheet sol.SpriteSheet

---@param level Level01
---@return Resources
local function loadResources(level)
    local resources = {}
    resources.platformSpriteSheet = level.store:createObject('sol.SpriteSheet', 'platform')
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
    local body_proto = level.store:createObject('sol.BodyPrototype', 'platform-01', sol.BodyType.STATIC)
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
    local body = level.scene:createBody(map_object.position, body_proto)
    if(not level.scene:setBodyShapeCurrentGraphic(body, 'platform', 'platform')) then
        print('Unable to set current graphics to platform-01')
    end
end

---@return Level01
local function createLevel()
    local store = sol.stores:createStore('level-01')
    local scene = store:createObject('sol.Scene', 'main')
    scene:setGravity({ x = 0, y = 8000 })
    scene:loadTileMap('tilemaps/level-01.tmx')
    local level = {
        scene = scene,
        store = store
    }
    local resources = loadResources(level)
    createPlatform(level, resources)
    return level
end

Level01 = {
    createLevel = createLevel
}

return Level01
