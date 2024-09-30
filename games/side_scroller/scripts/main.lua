local resources = require 'resources.all'

local global_store = sol.stores:createStore('global')
resources.init(global_store)

local function runLevel01()
    local level = require('level-01')
    local run = level.run(global_store)
    -- run.destroy()
end

runLevel01()
