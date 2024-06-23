local function createLevel()
    local main_scene_name = 'main'
    local scene = sol.world:createScene(main_scene_name)
    scene:loadTileMap('tilemaps/level-01.tmx')
    return {
        scene = scene,
        name = main_scene_name
    }
end

Level01 = {
    createLevel = createLevel
}

return Level01
