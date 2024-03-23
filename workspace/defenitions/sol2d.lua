---@meta

---@class Sol2D
---@field Scancode sol.Scancode
---@field BodyType sol.BodyType
---@field BodyShapeType sol.BodyShapeType
---@field world sol.World
---@field heartbeat sol.Heartbeat
---@field keyboard sol.Keyboard
---@field TileMapObjectType sol.TileMapObjectType
sol = {}

---@type any
self = {}

---@class Point
---@field x number
---@field y number

---@class Size
---@field w number
---@field h number

---@class Rectangle
---@field x number
---@field y number
---@field w number
---@field h number

---@class Color 
---@field r integer
---@field g integer
---@field b integer
---@field a integer?

---@class SpriteOptions 
---@field colorToAlpha Color?
---@field rect Rectangle?

---@class SpriteSheetOptions
---@field spriteWidth integer
---@field spriteHeight integer
---@field rowCount integer
---@field colCount integer
---@field marginTop integer?
---@field marginLeft integer?
---@field horizontalSpacing integer?
---@field verticalSpacing integer?
---@field colorToAlpha Color?

---@class ShapeGraphicOptions 
---@field position Point?
---@field size Size?
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?

---@class BodyOptions
---@field type integer?
---@field linearDamping number?
---@field angularDamping number?
---@field fixedRotation boolean?

---@class BodyShapeOptions
---@field isSensor boolean?
---@field density number?

---@class sol.World
local __world

---@param name string
---@return sol.Scene
function __world:createScene(name) end

---@param key string
---@return sol.Larder
function __world:createLarder(key) end

---@param key string
---@return sol.Larder | nil
function __world:getLarder(key) end

---@param key string
---@return boolean
function __world:deleteLarder(key) end

---@class sol.Scene
local __scene

---@param path string
---@return boolean
function __scene:loadTileMap(path) end

---@param id integer
---@return TileMapObject | nil
function __scene:getTileMapObjectById(id) end

---@param name string
---@return TileMapObject | nil
function __scene:getTileMapObjectByName(name) end

---@param position Point
---@param body_prototype sol.BodyPrototype
---@return integer
function __scene:createBody(position, body_prototype) end

---@param class string
---@param body_options BodyOptions?
---@param body_shape_options BodyShapeOptions?
function __scene:createBodiesFromMapObjects(class, body_options, body_shape_options) end

---@param body_id integer
---@param force_vector Point
function __scene:applyForce(body_id, force_vector) end

---@param body_id integer
---@param position Point
function __scene:setBodyPosition(body_id, position) end

---@param body_id integer
---@return boolean
function __scene:setFolowedBody(body_id) end

function __scene:resetFolowedBody() end

---@param body_id integer
---@param layer string
---@return boolean
function __scene:setBodyLayer(body_id, layer) end

---@param body_id integer
---@param shape_key string
---@param graphic_key string
---@return boolean
function __scene:setBodyShapeCurrentGraphic(body_id, shape_key, graphic_key) end

---@param body_id integer
---@param shape_key string
---@param graphic_key string
---@param flip_horizontally boolean
---@param flip_vertically boolean
---@return boolean
function __scene:flipBodyShapeGraphic(body_id, shape_key, graphic_key, flip_horizontally, flip_vertically) end

---@alias ContactCallback fun(contact: Contact)

---@param callback ContactCallback
function __scene:subscribeToBeginContact(callback) end

---@param callback ContactCallback
function __scene:subscribeToEndContact(callback) end

---@class Contact
---@field sideA ContactSide
---@field sideB ContactSide

---@class ContactSide
---@field bodyId integer
---@field shapeKey string
---@field tileMapObjectId integer?

---@class sol.Larder
local __larder;

---@param key string
---@param body_type integer
---@see sol.BodyType
---@return sol.BodyPrototype
function __larder:createBodyPrototype(key, body_type) end

---@param key string
---@return sol.BodyPrototype | nil
function __larder:getBodyPrototype(key) end

---@param key string
---@return boolean
function __larder:deleteBodyPrototype(key) end

---@param key string
---@return sol.Sprite
function __larder:createSprite(key) end

---@param key string
---@return sol.Sprite | nil
function __larder:getSprite(key) end

---@param key string
---@return boolean
function __larder:deleteSprite(key) end

---@param key string
---@return sol.SpriteSheet
function __larder:createSpriteSheet(key) end

---@param key string
---@return sol.SpriteSheet | nil
function __larder:getSpriteSheet(key) end

---@param key string
---@return boolean
function __larder:deleteSpriteSheet(key) end

---@param key string
---@return sol.SpriteAnimation
function __larder:createSpriteAnimation(key) end

---@param key string
---@return sol.SpriteAnimation | nil
function __larder:getSpriteAnimation(key) end

---@param key string
---@return boolean
function __larder:deleteSpriteAnimation(key) end

---@class sol.BodyPrototype
local __body_prototype

---@return integer # type of the body
---@see sol.BodyType
function __body_prototype:getType() end

---@param key string
---@param position Point
---@param radius number
---@return sol.BodyCircleShapePrototype
function __body_prototype:createCircleShape(key, position, radius) end

---@param key string
---@param rect Rectangle
---@return sol.BodyPolygonShapePrototype
function __body_prototype:createPolygonShape(key, rect) end

---@param key string
---@param ... Point points related to the position
---@return sol.BodyPolygonShapePrototype
function __body_prototype:createPolygonShape(key, ...) end

---@param path string
function __body_prototype:attachScript(path) end

---@class sol.BodyShapeType
---@field CIRCLE integer
---@field POLYGON integer

---@class sol.BodyShapePrototype
local __body_shape_prototype

---@param key string
---@param sprite sol.Sprite
---@param options ShapeGraphicOptions?
function __body_shape_prototype:addSprite(key, sprite, options) end

---@param key string
---@param sprite_animation sol.SpriteAnimation
---@param options ShapeGraphicOptions?
function __body_shape_prototype:addSpriteAnimation(key, sprite_animation, options) end

---@return integer
---@see sol.BodyShapeType
function __body_shape_prototype:getType() end

---@param is_sensor boolean
function __body_shape_prototype:setIsSensor(is_sensor) end

---@return boolean
function __body_shape_prototype:isSensor() end

---@param key string
function __body_shape_prototype:removeGraphic(key) end

---@class sol.BodyCircleShapePrototype : sol.BodyShapePrototype
local __body_circle_shape_prototype

---@return number
function __body_circle_shape_prototype:getRadius() end

---@class sol.BodyPolygonShapePrototype : sol.BodyShapePrototype
local __body_polygon_shape_prototype

---@class sol.Sprite
local __sprite

---@param path string
---@param options SpriteOptions?
---@return boolean
function __sprite:loadFromFile(path, options) end

---@class sol.SpriteSheet
local __sprite_sheet

---@param path string
---@param options SpriteSheetOptions
---@return boolean
function __sprite_sheet:loadFromFile(path, options) end

---@class sol.SpriteAnimation
local __sprite_animation

---@param during integer
---@param sprite sol.Sprite
---@return boolean
function __sprite_animation:addFrameFromSprite(during, sprite) end

---@param during integer
---@param sprite_sheet sol.SpriteSheet
---@param index integer
---@return boolean
function __sprite_animation:addFrameFromSpriteSheet(during, sprite_sheet, index) end

---@param during integer
---@param sprite_sheet sol.SpriteSheet
---@param indices integer[]
---@return boolean
function __sprite_animation:addFrames(during, sprite_sheet, indices) end

---@return Point[]
function __body_polygon_shape_prototype:getPoints() end

---@class sol.BodyType
---@field STATIC integer
---@field DYNAMIC integer
---@field KINEMATIC integer

---@class sol.Scancode
---@field RIGHT_ARROW integer
---@field LEFT_ARROW integer
---@field UP_ARROW integer
---@field DOWN_ARROW integer
---@field SPACE integer
---@field BACKSPACE integer
---@field TAB integer
---@field DELETE integer
---@field L_CTRL integer
---@field R_CTRL integer
---@field L_ALT integer
---@field R_ALT integer
---@field L_SHIFT integer
---@field R_SHIFT integer
---@field ESCAPE integer
---@field GRAVE integer
---@field _0 integer
---@field _1 integer
---@field _2 integer
---@field _3 integer
---@field _4 integer
---@field _5 integer
---@field _6 integer
---@field _7 integer
---@field _8 integer
---@field _9 integer
---@field KP_0 integer
---@field KP_1 integer
---@field KP_2 integer
---@field KP_3 integer
---@field KP_4 integer
---@field KP_5 integer
---@field KP_6 integer
---@field KP_7 integer
---@field KP_8 integer
---@field KP_9 integer
---@field KP_ENTER integer
---@field KP_MULTIPLY integer
---@field KP_DIVIDE integer
---@field KP_MINUS integer
---@field KP_PLUS integer
---@field KP_EQUALS integer
---@field KP_DECIMAL integer
---@field A integer
---@field B integer
---@field C integer
---@field D integer
---@field E integer
---@field F integer
---@field G integer
---@field H integer
---@field I integer
---@field J integer
---@field K integer
---@field L integer
---@field M integer
---@field N integer
---@field O integer
---@field P integer
---@field Q integer
---@field R integer
---@field S integer
---@field T integer
---@field U integer
---@field V integer
---@field W integer
---@field X integer
---@field Y integer
---@field Z integer
---@field SLASH integer
---@field BACKSLASH integer
---@field DECIMAL_SEPARATOR integer
---@field COMA integer
---@field END integer
---@field INSERT integer
---@field HOME integer
---@field PAGE_DOWN integer
---@field PAGE_UP integer
---@field F1 integer
---@field F2 integer
---@field F3 integer
---@field F4 integer
---@field F5 integer
---@field F6 integer
---@field F7 integer
---@field F8 integer
---@field F9 integer
---@field F10 integer
---@field F11 integer
---@field F12 integer
---@field F13 integer
---@field F14 integer
---@field F15 integer
---@field F16 integer
---@field F17 integer
---@field F18 integer
---@field F19 integer
---@field F20 integer
---@field F21 integer
---@field F22 integer
---@field F23 integer
---@field F24 integer
---@field PAUSE integer
---@field MINUS integer
---@field EQUALS integer
---@field CAPSLOCK integer
---@field NUMLOCK integer
---@field SCROLLOCK integer

---@class sol.Keyboard
local __keyboard

---@vararg integer
---@return boolean ...
function __keyboard:getState(...) end

---@class sol.Heartbeat
local __heartbeat

---@param callback function
function __heartbeat:subscribe(callback) end

---@class sol.TileMapObjectType
---@field CIRCLE integer
---@field POINT integer
---@field POLYGON integer
---@field POLYLINE integer
---@field TEXT integer

---@class TileMapObject
---@field id integer
---@field tile string?
---@field class string
---@field name string
---@field type integer
---@field position Point
---@field points? Point[]
