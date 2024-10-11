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
        buttonHull = 'hull-button',
        ---@param length integer
        ---@param body_type integer
        ---@return string
        platfrom = function(length, body_type)
            return 'platform-' .. body_type .. '-' .. length
        end
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
            hull = 'hull:button',
            sensor = 'sensor:button'
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
        platform = {
            main = 'platform'
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
