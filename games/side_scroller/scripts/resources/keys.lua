local ONE_WAY_PLATFORM_SHAPE_KEY = 'one-way-platform'

local keys_meta = {
    bodies = {
        player = 'player',
        flyingPlatform3 = 'flying-platform3'
    },
    shapes = {
        oneWayPlatfrom = ONE_WAY_PLATFORM_SHAPE_KEY,
        player = 'player-main'
    },
    shapeGraphics = {
        player = {
            idleLeft = 'player-idle-left',
            idleRight = 'player-idle-right',
            walkLeft = 'player-left',
            walkRight = 'player-right',
        },
        flyingPlatform3 = {
            main = 'flying-platform3-main'
        }
    },
    graphicPacks = {
        player = {
            idle = 'player-idle',
            walk = 'player-walk'
        },
        flyingPlatform3 = {
            main = 'flying-platform-3'
        }
    }
}

return setmetatable(keys_meta, {})
