local main_scene_name = 'main'
local scene = sol.world:createScene(main_scene_name)
scene:loadTileMap('tilemaps/level-01.tmx')
local main_fragment_id = sol.world:createFragment({ top = 0, bottom = 0, width = '100%', height = '100%' }) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
sol.world:bindFragment(main_fragment_id, main_scene_name)
