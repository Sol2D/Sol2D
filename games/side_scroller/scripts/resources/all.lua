local player = require 'resources.player'
local flying_platform = require 'resources.flying-platform'

return {
    ---@param store sol.Store
    init = function (store)
        player.init(store)
        flying_platform.init(store)
    end
}
