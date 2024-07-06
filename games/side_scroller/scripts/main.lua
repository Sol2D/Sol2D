require 'level-01'
require 'player'

---@param options Fragment
local function createFragment(options)
    return {
        id = sol.world:createFragment(options),
        bind = function(self, outlet)
            sol.world:bindFragment(self.id, outlet)
        end
    }
end

(function()
    local main_fragment = createFragment({}) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
    local level_01 = Level01.createLevel('level-01')
    level_01.scene:createBodiesFromMapObjects('obstacle')
    main_fragment:bind(level_01.name)

    local main_larder = sol.world:createLarder('main')
    local player_proto = Player.createPrototype(main_larder)  -- player_common_moudle.createPrototype(main_larder)
    local player_id = level_01.scene:createBody({ x = 410, y = 200 }, player_proto.proto)
    level_01.scene:setBodyShapeCurrentGraphic(
        player_id,
        player_proto.shapes.main,
        player_proto.graphics.idle_right
    )
    level_01.scene:setFollowedBody(player_id)
end)()
