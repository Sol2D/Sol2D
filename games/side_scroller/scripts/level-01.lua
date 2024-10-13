local level_common = require 'level-common'

local level_01 = {
    name = 'level-01',
    tileMap = 'tilemaps/level-01.tmx',
    music = 'sounds/level-01/level-01.wav'
}

local module = {}
module.__index = module

---@param on_finish function
function module.run(on_finish)
    return level_common.run(level_01, on_finish)
end

return setmetatable({}, module)
