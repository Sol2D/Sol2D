local player = require 'resources.player'
local flying_platform = require 'resources.flying-platform'
local sounds = require 'resources.sounds'

return {
    ---@param store sol.Store
    init = function (store)
        player.init(store)
        flying_platform.init(store)
        sounds.init(store)
    end
}
