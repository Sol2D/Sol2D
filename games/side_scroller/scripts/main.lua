local global_resources = require 'resources'

local function runLevel_02()
    local level = require 'level-02'
    local run;
    run = level.run(function()
        run.destroy()
    end)
end

local function runLevel_01()
    local level = require 'level-01'
    local run;
    run = level.run(function ()
        run.destroy()
        runLevel_02()
    end)
end

global_resources.initialize()
runLevel_01()
