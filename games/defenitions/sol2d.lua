---@meta

---@class Sol2D
---@field Scancode sol.Scancode
---@field BodyType sol.BodyType
---@field BodyShapeType sol.BodyShapeType
---@field world sol.World
---@field heartbeat sol.Heartbeat
---@field keyboard sol.Keyboard
---@field TileMapObjectType sol.TileMapObjectType
---@field DimensionUnit sol.DimensionUnit
---@field WidgetState sol.WidgetState
---@field HorizontalTextAlignment sol.HorizontalTextAlignment
---@field VerticalTextAlignment sol.VerticalTextAlignment
sol = nil

---@type BodyContext
self = nil

---@class BodyContext
---@field bodyId integer
---@field scene sol.Scene
---@field arg any?

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
---@field autodetectRect boolean? default false

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
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?

---@class SpriteAnimationOptions
---@field duration integer? # milliseconds
---@field position? Point

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

---@param name string
---@return sol.Form
function __world:createForm(name) end

---@param key string
---@return sol.Larder
function __world:createLarder(key) end

---@param key string
---@return sol.Larder | nil
function __world:getLarder(key) end

---@param key string
---@return boolean
function __world:deleteLarder(key) end

---@param fragment Fragment
---@return integer
function __world:createFragment(fragment) end

---@param fragment_id integer
---@param fragment Fragment
---@return boolean
function __world:updateFragment(fragment_id, fragment) end

---@param fragment_id integer
---@return Fragment?
function __world:getFragment(fragment_id) end

---@param fragment_id integer
---@return boolean
function __world:deleteFragment(fragment_id) end

---@param fragment_id integer
---@param name string A name of the scene or form
---@return boolean
function __world:bindFragment(fragment_id, name) end

---@class Fragment
---@field top Dimension?
---@field right Dimension?
---@field left Dimension?
---@field bottom Dimension?
---@field width Dimension?
---@field height Dimension?
---@field zIndex integer? default: 0
---@field isVisible boolean? default: true

---@class DimensionTable
---@field unit integer
---@field value integer

---@alias Dimension DimensionTable | string | number

---@class sol.DimensionUnit
---@field PIXEL integer
---@field PERCENT integer

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

---@param position Point | nil
---@param body_prototype sol.BodyPrototype
---@param argument? any
---@return integer
function __scene:createBody(position, body_prototype, argument) end

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
---@returns Point | nil
function __scene:getBodyPosition(body_id) end

---@param body_id integer
---@return boolean
function __scene:setFollowedBody(body_id) end

function __scene:resetFollowedBody() end

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
---@return integer subscription ID
function __scene:subscribeToBeginContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromBeginContact(subscription_id) end

---@param callback ContactCallback
---@return integer subscription ID
function __scene:subscribeToEndContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromEndContact(subscription_id) end

---@param body_id integer
---@param destination Point
---@return Point[] | nil
function __scene:findPath(body_id, destination) end

---@class sol.Form
local __form

---@param text string?
---@return sol.Label
function __form:createLabel(text) end

---@param text string?
---@return sol.Button
function __form:createButton(text) end

---@class sol.WidgetState
---@field DEFAULT integer
---@field FOCUSED integer
---@field ACTIVATED integer

---@class sol.HorizontalTextAlignment
---@field NONE integer
---@field BEGIN integer
---@field CENTER integer
---@field END integer

---@class sol.VerticalTextAlignment
---@field NONE integer
---@field TOP integer
---@field CENTER integer
---@field BOTTOM integer

---@class WidgetPadding
---@field top Dimension?
---@field right Dimension?
---@field bottom Dimension?
---@field left Dimension?

---@class sol.Widget
local __widget

---@param value Dimension
function __widget:setX(value) end

---@param value Dimension
function __widget:setY(value) end

---@param value Dimension
function __widget:setWidth(value) end

---@param value Dimension
function __widget:setHeight(value) end

---@param color Color
---@param widget_state integer?
---@see sol.WidgetState
function __widget:setBackgroundColor(color, widget_state) end

---@param color Color
---@param widget_state integer?
---@see sol.WidgetState
function __widget:setForegroundColor(color, widget_state) end

---@param color Color
---@param widget_state integer?
---@see sol.WidgetState
function __widget:setBorderColor(color, widget_state) end

---@param width number
---@param widget_state integer?
---@see sol.WidgetState
function __widget:setBorderWidth(width, widget_state) end

---@param padding WidgetPadding | Dimension
---@param widget_state integer?
---@see sol.WidgetState
function __widget:setPadding(padding, widget_state) end

---@class sol.Label: sol.Widget
local __label

---@param text string?
function __label:setText(text) end

---@param font sol.Font
function __label:setFont(font) end

---@param alignment integer
---@param widget_state integer?
---@see sol.HorizontalTextAlignment
function __label:setHorizontalTextAlignment(alignment, widget_state) end

---@param alignment integer
---@param widget_state integer?
---@see sol.VerticalTextAlignment
function __label:setVerticalTextAlignment(alignment, widget_state) end

---@class sol.Button: sol.Label
local __button

---@param callback function
---@return integer subscription ID
function __button:subscribeOnClick(callback) end

---@param subscription_id integer
function __button:unsubscribeOnClick(subscription_id) end

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

---@param file_path string
---@param size integer
---@return sol.Font | nil
function __larder:getFont(file_path, size) end

---@param file_path string
---@param size integer
function __larder:freeFont(file_path, size) end

---@param file_path string
---@return sol.SoundEffect | nil
function __larder:getSoundEffect(file_path) end

---@param file_path string
function __larder:freeSoundEffect(file_path) end

---@param file_path string
---@return sol.Music | nil
function __larder:getMusic(file_path) end

---@param file_path string
function __larder:freeMusic(file_path) end

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

---@return Rectangle
function __sprite:getSourceRect() end

---@return Size
function __sprite:getDestinationSize() end

---@param size Size
function __sprite:setDestinationSize(size) end

function __sprite:scale() end

---@param scale_factor number
function __sprite:scale(scale_factor) end

---@param scale_factor_x number
---@param scale_factor_y number
function __sprite:scale(scale_factor_x, scale_factor_y) end

---@class sol.SpriteSheet
local __sprite_sheet

---@param path string
---@param options SpriteSheetOptions
---@return boolean
function __sprite_sheet:loadFromFile(path, options) end

---@class sol.SpriteAnimation
local __sprite_animation

---@param sprite sol.Sprite
---@param options? SpriteAnimationOptions
---@return boolean
function __sprite_animation:addFrameFromSprite(sprite, options) end

---@param sprite_sheet sol.SpriteSheet
---@param index integer
---@param options? SpriteAnimationOptions
---@return boolean
function __sprite_animation:addFrameFromSpriteSheet(sprite_sheet, index, options) end

---@param sprite_sheet sol.SpriteSheet
---@param indices integer[]
---@param options? SpriteAnimationOptions
---@return boolean
function __sprite_animation:addFrames(sprite_sheet, indices, options) end

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

---@class sol.Font
local __font

---@class sol.SoundEffect
local __sound_effect

---@param channel integer?
---@return boolean
function __sound_effect:play(channel) end

---@param iteration_count integer
---@param channel integer?
---@return boolean
function __sound_effect:loop(iteration_count, channel) end

---@class sol.Music
local __music

---@return boolean
function __music:play() end

---@param iteration_count integer
---@return boolean
function __music:loop(iteration_count) end
