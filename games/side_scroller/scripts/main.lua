local createLevel01 = require 'level-01'
local resources = require 'resources'

local global_store = sol.stores:createStore('global')
resources.initialize(global_store)
local level_view = global_store:createView('level')
local scene_fragment = level_view:createFragment({}) -- TODO: optional parameter
sol.window:setView(level_view)

local level_01 = createLevel01()

level_01.__gc = function ()
    print('level destroyed')
end

level_01:run(level_view, scene_fragment)

-- TODO: destroy global_store
