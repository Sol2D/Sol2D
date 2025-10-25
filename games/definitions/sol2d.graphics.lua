---@meta

---@class sol.GraphicsPackDefinition
---@field position sol.Point?
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?
---@field animationIterations integer?
---@field frames sol.GraphicsPackFrameDefinition[]

---@class sol.GraphicsPackFrameDefinition
---@field duration integer? milliseconds
---@field isVisible boolean? default is true
---@field sprites sol.GraphicsPackSpriteDefinition[]

---@class sol.GraphicsPackSpriteDefinition
---@field isVisible boolean? default is true
---@field position sol.Point?
---@field scaleFactor sol.Point?
---@field zIndex integer? default is 0
---@field sprite sol.Sprite | { spriteSheet: sol.SpriteSheet, spriteIndex: integer }

---@class SpriteOptions
---@field colorToAlpha sol.Color?
---@field rect sol.Rectangle?
---@field autodetectRect boolean? default false

---@class sol.SpriteSheetOptions
---@field spriteWidth integer
---@field spriteHeight integer
---@field rowCount integer
---@field colCount integer
---@field marginTop integer?
---@field marginLeft integer?
---@field horizontalSpacing integer?
---@field verticalSpacing integer?
---@field colorToAlpha sol.Color?

---@class sol.Sprite
local __sprite

---@param path string
---@param options SpriteOptions?
---@return boolean
function __sprite:loadFromFile(path, options) end

---@return boolean
function __sprite:isValid() end

---@param size sol.Size
function __sprite:scaleTo(size) end

function __sprite:scale() end

---@param scale_factor number
function __sprite:scale(scale_factor) end

---@param scale_factor_x number
---@param scale_factor_y number
function __sprite:scale(scale_factor_x, scale_factor_y) end

---@class sol.SpriteSheet
local __sprite_sheet

---@param path string
---@param options sol.SpriteSheetOptions?
---@return boolean
function __sprite_sheet:loadFromFile(path, options) end

---@class sol.GraphicsPack
local __graphics_pack

---@param definition sol.GraphicsPackFrameDefinition
---@return integer # index of the new frame
function __graphics_pack:addFrame(definition) end

---@param frame_index integer # position to insert
---@param definition sol.GraphicsPackFrameDefinition
---@return integer # index of the new frame
function __graphics_pack:insertFrame(frame_index, definition) end

---@param frame_index integer # position to insert
---@return boolean
function __graphics_pack:removeFrame(frame_index) end

---@param frame_index integer
---@param visibility boolean
---@return boolean
function __graphics_pack:setFrameVisibility(frame_index, visibility) end

---@param frame_index integer
---@return boolean | nil
function __graphics_pack:isFrameVisible(frame_index) end

---@param frame_index integer
---@param duration integer # milliseconds
---@return boolean
function __graphics_pack:setFrameDuration(frame_index, duration) end

---@param frame_index integer
---@return integer | nil # milliseconds
function __graphics_pack:getFrameDuration(frame_index) end

---@return integer
function __graphics_pack:getCurrentFrameIndex() end

---@return boolean
---@param frame_index integer
function __graphics_pack:setCurrentFrameIndex(frame_index) end

---@return integer
function __graphics_pack:getCurrentAnimationIteration() end

---@return boolean
function __graphics_pack:switchToFirstVisibleFrame() end

---@return boolean
function __graphics_pack:switchToNextVisibleFrame() end
