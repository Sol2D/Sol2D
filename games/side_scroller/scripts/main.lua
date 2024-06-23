local level_01_module = require('level-01')

local function createFragment(options)
    return {
        id = sol.world:createFragment(options),
        bind = function (fragment, outlet)
            sol.world:bindFragment(fragment.id, outlet)
        end
    }
end

local main_fragment = createFragment({ top = 0, bottom = 0, width = '100%', height = '100%' }) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
local level_01 = level_01_module.createLevel()
main_fragment:bind(level_01.name)
