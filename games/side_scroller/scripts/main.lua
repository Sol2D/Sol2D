local Game = require 'game'
local createLevel01 = require 'level-01'
local createLevel02 = require 'level-02'
local resources = require 'resources'

local global_store = sol.stores:createStore('global')
resources.initialize(global_store)
local level_view = global_store:createView('level')
sol.window:setView(level_view)

local game = Game.new(global_store)

local function runLevel02()
    local level_02 = createLevel02(game)
    level_02:run(level_view, function()
        level_02:destroy()
        -- TODO: finish
    end)
end

local function runLevel01()
    local level_01 = createLevel01(game)
    level_01.__gc = function()
        print('level destroyed')
    end
    level_01:run(level_view, function()
        -- level_01:destroy()
        runLevel02()
    end)
end

runLevel01()

-- TODO: destroy global_store
