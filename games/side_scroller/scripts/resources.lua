local keys = {
    sprites = {
        button = {
            CAP = 'cap:button',
            HULL = 'hull:button',
        },
        knight = {
            idle = {
                'idle-1:knight',
                'idle-2:knight',
                'idle-3:knight',
                'idle-4:knight',
                'idle-5:knight',
                'idle-6:knight',
                'idle-7:knight',
                'idle-8:knight',
                'idle-9:knight',
                'idle-10:knight',
                rect = { x = 58, y = 42, w = 434, h = 615 }
            },
            walk = {
                'walk-1:knight',
                'walk-2:knight',
                'walk-3:knight',
                'walk-4:knight',
                'walk-5:knight',
                'walk-6:knight',
                'walk-7:knight',
                'walk-8:knight',
                'walk-9:knight',
                'walk-10:knight',
                rect = { x = 55, y = 31, w = 444, h = 662 }
            },
            jump = {
                'jump-1:knight',
                'jump-2:knight',
                'jump-3:knight',
                'jump-4:knight',
                'jump-5:knight',
                'jump-6:knight',
                'jump-7:knight',
                'jump-8:knight',
                'jump-9:knight',
                'jump-10:knight',
                rect = { x = 23, y = 27, w = 480, h = 668 }
            },
            attack = {
                'attack-1:knight',
                'attack-2:knight',
                'attack-3:knight',
                'attack-4:knight',
                'attack-5:knight',
                'attack-6:knight',
                'attack-7:knight',
                'attack-8:knight',
                'attack-9:knight',
                'attack-10:knight',
                rect = { x = 43, y = 40, w = 495, h = 620 }
            },
        },
        robot = {
            dead = {
                'dead-1:robot',
                'dead-2:robot',
                'dead-3:robot',
                'dead-4:robot',
                'dead-5:robot',
                'dead-6:robot',
                'dead-7:robot',
                'dead-8:robot',
                'dead-9:robot',
                'dead-10:robot'
            },
            idle = {
                'idle-1:robot',
                'idle-2:robot',
                'idle-3:robot',
                'idle-4:robot',
                'idle-5:robot',
                'idle-6:robot',
                'idle-7:robot',
                'idle-8:robot',
                'idle-9:robot',
                'idle-10:robot',
                rect = { x = 127, y = 53, w = 273, h = 468 }
            },
            jump = {
                'jump-1:robot',
                'jump-2:robot',
                'jump-3:robot',
                'jump-4:robot',
                'jump-5:robot',
                'jump-6:robot',
                'jump-7:robot',
                'jump-8:robot',
                'jump-9:robot',
                'jump-10:robot'
            },
            jumpMelee = {
                'jump-melee-1:robot',
                'jump-melee-2:robot',
                'jump-melee-3:robot',
                'jump-melee-4:robot',
                'jump-melee-5:robot',
                'jump-melee-6:robot',
                'jump-melee-7:robot',
                'jump-melee-8:robot'
            },
            jumpShoot = {
                'jump-shoot-1:robot',
                'jump-shoot-2:robot',
                'jump-shoot-3:robot',
                'jump-shoot-4:robot',
                'jump-shoot-5:robot'
            },
            melee = {
                'melee-1:robot',
                'melee-2:robot',
                'melee-3:robot',
                'melee-4:robot',
                'melee-5:robot',
                'melee-6:robot',
                'melee-7:robot',
                'melee-8:robot'
            },
            run = {
                'run-1:robot',
                'run-2:robot',
                'run-3:robot',
                'run-4:robot',
                'run-5:robot',
                'run-6:robot',
                'run-7:robot',
                'run-8:robot',
                rect = { x = 78, y = 54, w = 366, h = 495 }
            },
            runShoot = {
                'run-shoot-1:robot',
                'run-shoot-2:robot',
                'run-shoot-3:robot',
                'run-shoot-4:robot',
                'run-shoot-5:robot',
                'run-shoot-6:robot',
                'run-shoot-7:robot',
                'run-shoot-8:robot',
                'run-shoot-9:robot'
            },
            shoot = {
                'shoot-1:robot',
                'shoot-2:robot',
                'shoot-3:robot',
                'shoot-4:robot'
            },
            slide = {
                'slide-1:robot',
                'slide-2:robot',
                'slide-3:robot',
                'slide-4:robot',
                'slide-5:robot',
                'slide-6:robot',
                'slide-7:robot',
                'slide-8:robot',
                'slide-9:robot',
                'slide-10:robot'
            }
        }
    },
    spriteSheets = {
        PLATFORM = 'platform',
        ROBOT_IDLE = 'robot:idle',
        ROBOT_RUN = 'robot:run'
    },
    soundEffects = {
        ARMOR = 'armor',
        SWING = 'swing'
    }
}

---@param store sol.Store
---@param key string
---@param file string
---@param options SpriteOptions?
local function loadSprite(store, key, file, options)
    local sprite = store:createSprite(key)
    if not sprite:loadFromFile(file, options) then
        error('Unable to load sprite form ' .. file)
    end
end

---@param store sol.Store
---@param key string
---@param file string
---@param options sol.SpriteSheetOptions?
local function loadSpriteSheet(store, key, file, options)
    local sprite_sheet = store:createSpriteSheet(key)
    if (not sprite_sheet:loadFromFile(file, options)) then
        error('Unable to load sprite sheet from ' .. file)
    end
end

---@param store sol.Store
---@param file string
local function loadAtlas(store, key, file)
    local sprite_sheet = store:createSpriteSheet(key)
    if not sprite_sheet:loadFromAtlas(file) then
        error('Unable to load sprite sheet from atlas ' .. file)
    end
end

---@param store sol.Store
---@param key string
---@param file string
local function loadSoundEffect(store, key, file)
    local sound_effect = store:createAudio(key, file, true)
    if not sound_effect then
        error('Unable to load sound effect form ' .. file)
    end
end

---@param store sol.Store
local function load(store)
    loadSprite(store, keys.sprites.button.CAP, 'sprites/button/button.png')
    loadSprite(store, keys.sprites.button.HULL, 'sprites/button/hull.png')
    for index, key in ipairs(keys.sprites.knight.idle) do
        loadSprite(
            store,
            key,
            'sprites/knight/idle (' .. index .. ').png',
            { rect = keys.sprites.knight.idle.rect }
        )
    end
    for index, key in ipairs(keys.sprites.knight.walk) do
        loadSprite(
            store,
            key,
            'sprites/knight/walk (' .. index .. ').png',
            { rect = keys.sprites.knight.walk.rect }
        )
    end
    for index, key in ipairs(keys.sprites.knight.jump) do
        loadSprite(
            store,
            key,
            'sprites/knight/jump (' .. index .. ').png',
            { rect = keys.sprites.knight.jump.rect }
        )
    end
    for index, key in ipairs(keys.sprites.knight.attack) do
        loadSprite(
            store,
            key,
            'sprites/knight/attack (' .. index .. ').png',
            { rect = keys.sprites.knight.attack.rect }
        )
    end

    loadSpriteSheet(
        store,
        keys.spriteSheets.PLATFORM,
        'sprites/platform/platform.png',
        { colCount = 3, rowCount = 1, spriteWidth = 128, spriteHeight = 64 }
    )

    loadAtlas(
        store,
        keys.spriteSheets.ROBOT_IDLE,
        'robot/idle.xml'
    )

    loadAtlas(
        store,
        keys.spriteSheets.ROBOT_RUN,
        'robot/run.xml'
    )

    loadSoundEffect(store, keys.soundEffects.ARMOR, 'sounds/sound-effects/rpg-sound-pack/inventory/armor-light.wav')
    loadSoundEffect(store, keys.soundEffects.SWING, 'sounds/sound-effects/rpg-sound-pack/battle/swing.wav')
end

local function construct()
    local resources_store = nil

    local function getStore()
        if not resources_store then
            error('Global store is not initialized')
        end
        return resources_store
    end

    local resources = {
        keys = keys
    }
    resources.__index = resources

    ---@param store sol.Store
    function resources.initialize(store)
        if resources_store then
            error('Global resources already initialized')
        else
            resources_store = store
            load(resources_store)
        end
    end

    ---@param key string
    ---@return sol.Sprite
    function resources.getSprite(key)
        local sprite = getStore():getSprite(key)
        if not sprite then
            error('Sprite ' .. key .. ' not found')
        end
        return sprite
    end

    ---@param key string
    ---@return sol.SpriteSheet
    function resources.getSpriteSheet(key)
        local sprite_sheet = getStore():getSpriteSheet(key)
        if not sprite_sheet then
            error('Sprite sheet' .. key .. ' not found')
        end
        return sprite_sheet
    end

    ---@param key string
    ---@return sol.Audio
    function resources.getSoundEffect(key)
        local sound_effect = getStore():getAudio(key)
        if not sound_effect then
            error('Audio ' .. key .. ' not found')
        end
        return sound_effect
    end

    return resources
end

local resources = construct()
return setmetatable({}, resources)
