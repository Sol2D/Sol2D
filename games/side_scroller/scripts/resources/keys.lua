local ONE_WAY_PLATFORM_SHAPE_KEY = 'one-way-platform'

local keys_meta = {
    tilemap = {
        layers = {
            ground1 = 'ground-1',
            ground2 = 'ground-2',
            places = 'places'
        }
    },
    bodies = {
        player = 'player',
        flyingPlatform3 = 'flying-platform3'
    },
    shapes = {
        oneWayPlatfrom = {
            main = ONE_WAY_PLATFORM_SHAPE_KEY
        },
        player = {
            main = 'main:player',
            bottomSensor = 'sensor-bottom:player'
        }
    },
    shapeGraphics = {
        player = {
            idleLeft = 'idle-left:player',
            idleRight = 'idle-right:player',
            walkLeft = 'walk-left:player',
            walkRight = 'walk-right:player',
            jumpLeft = 'jump-left:player',
            jumpRight = 'jump-right:player'
        },
        flyingPlatform3 = {
            main = 'main:flying-platform3'
        }
    }
}

return setmetatable(keys_meta, {})
