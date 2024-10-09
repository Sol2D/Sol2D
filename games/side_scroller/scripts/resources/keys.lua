local ONE_WAY_PLATFORM_SHAPE_KEY = 'one-way-platform'

local keys_meta = {
    stores = {
        main = 'global',
        level01 = '1:level',
        level02 = '2:level'
    },
    tilemap = {
        level01 = {
            layers = {
                background = 'background',
                ground1 = 'ground-1',
                ground2 = 'ground-2',
                places = 'places'
            }
        },
        level02 = {
            layers = {
                background = 'background',
                ground1 = 'ground-1',
                places = 'places'
            }
        }
    },
    bodies = {
        player = 'player',
        flyingPlatform3 = 'flying-platform3',
        button = 'button',
        buttonHull = 'hull-button'
    },
    shapes = {
        oneWayPlatfrom = {
            main = ONE_WAY_PLATFORM_SHAPE_KEY
        },
        obstacle = {
            main = 'obstacle'
        },
        player = {
            main = 'main:player',
            bottomSensor = 'sensor-bottom:player'
        },
        water = {
            main = 'water'
        },
        button = {
            main = 'main:button',
            hull = 'hull:button'
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
        },
        button = {
            main = 'main:button',
            hull = 'main:button'
        }
    },
    soundEffects = {
        armor = 'armor',
        swing = 'swing'
    }
}

return setmetatable(keys_meta, {})
