require 'level-01'
require 'player'

---@param view sol.View
---@param options Fragment
local function createFragment(view, options)
    return {
        id = view:createFragment(options),
        bind = function(self, target)
            view:bindFragment(self.id, target)
        end
    }
end

(function()
    local main_store = sol.stores:createStore('main')
    local main_view = main_store:createObject('sol.View', 'main')
    local main_fragment = createFragment(main_view, {}) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
    local level_01 = Level01.createLevel()
    level_01.scene:createBodiesFromMapObjects('obstacle')
    main_fragment:bind(level_01.scene)
    sol.window:setView(main_view)

    local player_proto = Player.createPrototype(main_store)
    local player_id = level_01.scene:createBody({ x = 410, y = 200 }, player_proto.proto)
    level_01.scene:setBodyShapeCurrentGraphic(
        player_id,
        player_proto.shapes.main,
        player_proto.graphics.idle_right
    )
    level_01.scene:setFollowedBody(player_id)
end)()
