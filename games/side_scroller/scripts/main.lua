local resources = require 'resources.all'
local keys = require 'resources.keys'

local global_store = sol.stores:createStore(keys.stores.main)
resources.init(global_store)

local function runLevel02()
    local level = require('level-02')
    local run = level.run(global_store)
    -- run.destroy()
end

local function runLevel01()
    local level = require('level-01')
    local run
    run = level.run(global_store, function ()
        run.destroy()
        runLevel02()
    end)
end

runLevel01()
