local createBaseLevel = require 'level'

local STORE_KEY = 'level-02'

local function createLevel02()
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
        if not scene:loadTileMap('tilemaps/level-02.tmx') then
            error('Unable to load level-02.tmx')
        end
        return scene
    end

    function level:createMusic()
        return store:createMusic('level-02', 'sounds/level-01/level-01.wav')
    end

    function level:destroy()
        sol.stores:deleteStore(STORE_KEY)
    end

    return setmetatable(level, createBaseLevel())
end

return createLevel02
