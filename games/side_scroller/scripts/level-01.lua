---@param name string
---@return { scene: sol.Scene, name: string }
local function createLevel(name)
    local scene = sol.world:createScene(name)
    scene:loadTileMap('tilemaps/level-01.tmx')
    return {
        scene = scene,
        name = name
    }
end

Level01 = {
    createLevel = createLevel
}

return Level01
