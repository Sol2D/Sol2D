local createBaseLevel = require 'level'

local STORE_KEY = 'level-02'

--@param game Game
local function createLevel02(game)
    local store = sol.stores:createStore(STORE_KEY)
    local level = {}

    ---@param view sol.View
    ---@return sol.Scene
    function level:createScene(view)
        local scene = store:createScene(
            'level-02',
            view:getLayout(),
            {
                gravity = { x = 0, y = 80 },
                metersPerPixel = self.METERS_PER_PIXEL
            }
        )
        if not scene:loadTileMap('tilemaps/level-02.tmx') then
            error('Unable to load level-02.tmx')
        end
        return scene
    end

    function level:createMusic()
        return store:createAudio('level-02', 'sounds/level-01/level-01.wav')
    end

    function level:destroy()
        sol.stores:deleteStore(STORE_KEY)
    end

    return setmetatable(level, createBaseLevel(game))
end

return createLevel02
