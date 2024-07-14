---@return { scene: sol.Scene, store: sol.Store }
local function createLevel()
    local store = sol.stores:createStore('level-01')
    local scene = store:createObject('sol.Scene', 'main')
    scene:loadTileMap('tilemaps/level-01.tmx')
    return {
        scene = scene,
        store = store
    }
end

Level01 = {
    createLevel = createLevel
}

return Level01
