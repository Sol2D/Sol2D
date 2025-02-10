local createBaseLevel = require 'level'


local STORE_KEY = 'level-01'

local function createLevel01()
    local store = sol.stores:createStore(STORE_KEY)
    local level = {}

    ---@return sol.Scene
    function level:createScene()
        local scene = store:createScene(
            'level-01',
            {
                gravity = { x = 0, y = 80 },
                metersPerPixel = self.METERS_PER_PIXEL
            }
        )
        if not scene:loadTileMap('tilemaps/level-01.tmx') then
            error('Unable to load level-01.tmx')
        end
        return scene
    end

    function level:destroy()
        sol.stores:deleteStore(STORE_KEY)
    end

    return setmetatable(level, createBaseLevel())
end

return createLevel01
