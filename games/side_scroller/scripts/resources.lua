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
            }
        }
    },
    spriteSheets = {
        PLATFORM = 'platform'
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
---@param options SpriteSheetOptions?
local function loadSpriteSheet(store, key, file, options)
    local sprite_sheet = store:createSpriteSheet(key)
    if (not sprite_sheet:loadFromFile(file, options)) then
        error('Unable to load sprite sheet from ' .. file)
    end
end

---@param store sol.Store
---@param key string
---@param file string
local function loadSoundEffect(store, key, file)
    local sound_effect = store:createSoundEffect(key, file)
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

    loadSpriteSheet(
        store,
        keys.spriteSheets.PLATFORM,
        'sprites/platform/platform.png',
        { colCount = 3, rowCount = 1, spriteWidth = 128, spriteHeight = 64 }
    )

    loadSoundEffect(store, keys.soundEffects.ARMOR, 'sounds/sound-effects/rpg-sound-pack/inventory/armor-light.wav')
    loadSoundEffect(store, keys.soundEffects.SWING, 'sounds/sound-effects/rpg-sound-pack/battle/swing.wav')
end

local function construct()
    local STORE_KEY = 'global-rsc'
    local store = sol.stores:getStore(STORE_KEY)

    local function getStore()
        if not store then
            error('Global store is not initialized')
        end
        return store
    end

    local resources = {
        keys = keys
    }
    resources.__index = resources

    function resources.initialize()
        if store then
            return
        else
            store = sol.stores:createStore(STORE_KEY)
            load(store)
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

    function resources.destroy()
        if store then
            sol.stores:deleteStore(STORE_KEY)
            store = nil
        end
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
    ---@return sol.SoundEffect
    function resources.getSoundEffect(key)
        local sound_effect = getStore():getSoundEffect(key)
        if not sound_effect then
            error('Sound effect' .. key .. ' not found')
        end
        return sound_effect
    end

    return resources
end

local resources = construct()
return setmetatable({}, resources)
