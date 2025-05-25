---@meta

---@class sol.StoreManager
local __store_manager

---@param key string
---@return sol.Store
function __store_manager:createStore(key) end

---@param key string
---@return sol.Store | nil
function __store_manager:getStore(key) end

---@param key string
---@return boolean
function __store_manager:deleteStore(key) end

---@class sol.Store
local __store

---@param key string
---@return sol.View
function __store:createView(key) end

---@param key string
---@return sol.View | nil
function __store:getView(key) end

---@param key string
---@return boolean
function __store:freeView(key) end

---@param key string
---@param style sol.Style?
---@return sol.View
function __store:createView(key, style) end

---@param key string
---@return sol.View | nil
function __store:getView(key) end

---@param key string
---@return boolean
function __store:freeView(key) end

---@param key string
---@param node sol.Node
---@param options sol.SceneOptions?
---@return sol.Scene
function __store:createScene(key, node, options) end

---@param key string
---@return sol.Scene | nil
function __store:getScene(key) end

---@param key string
---@return boolean
function __store:freeScene(key) end

---@param key string
---@return boolean
function __store:freeForm(key) end

---@param key string
---@return sol.UI
function __store:createUI(key) end

---@param key string
---@return sol.UI | nil
function __store:getUI(key) end

---@param key string
---@return boolean
function __store:freeUI(key) end

---@param key string
---@return sol.Sprite
function __store:createSprite(key) end

---@param key string
---@return sol.Sprite | nil
function __store:getSprite(key) end

---@param key string
---@return boolean
function __store:freeSprite(key) end

---@param key string
---@return sol.SpriteSheet
function __store:createSpriteSheet(key) end

---@param key string
---@return sol.SpriteSheet | nil
function __store:getSpriteSheet(key) end

---@param key string
---@return boolean
function __store:freeSpriteSheet(key) end

---@param key string
---@param file_path string
---@return sol.SoundEffect
function __store:createSoundEffect(key, file_path) end

---@param key string
---@return sol.SoundEffect | nil
function __store:getSoundEffect(key) end

---@param key string
---@return boolean
function __store:freeSoundEffect(key) end

---@param key string
---@param file_path string
---@return sol.Music | nil
function __store:createMusic(key, file_path) end

---@param key string
---@return sol.Music | nil
function __store:getMusic(key) end

---@param key string
---@return boolean
function __store:freeMusic(key) end

---@param key string
---@param file_path string
---@param font_size integer
---@return sol.Font
function __store:createFont(key, file_path, font_size) end

---@param key string
---@return sol.Font | nil
function __store:getFont(key) end

---@param key string
---@return boolean
function __store:freeFont(key) end
