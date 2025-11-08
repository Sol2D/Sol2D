local createBaseLevel = require 'level'

local STORE_KEY = 'level-01'

--@param game Game
local function createLevel01(game)
    local store = sol.stores:createStore(STORE_KEY)
    local level = {}

    ---@param view sol.View
    ---@return sol.Scene
    function level:createScene(view)
        local scene = store:createScene(
            'level-01',
            view:getLayout(),
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

    function level:createMusic()
        return store:createAudio('level-01', 'sounds/level-01/level-01.wav')
    end

    function level:destroy()
        sol.stores:deleteStore(STORE_KEY)
    end

    return setmetatable(level, createBaseLevel(game))
end

return createLevel01
