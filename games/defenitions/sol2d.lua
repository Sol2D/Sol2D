---@meta

---@class Sol2D
---@field Scancode sol.Scancode
---@field BodyType sol.BodyType
---@field BodyShapeType sol.BodyShapeType
---@field keyboard sol.Keyboard
---@field stores sol.StoreManager
---@field window sol.Window
---@field TileMapObjectType sol.TileMapObjectType
---@field DimensionUnit sol.DimensionUnit
---@field WidgetState sol.WidgetState
---@field HorizontalTextAlignment sol.HorizontalTextAlignment
---@field VerticalTextAlignment sol.VerticalTextAlignment
sol = nil

---@type ScriptContext
script = nil

---@class ScriptContext
---@field body sol.Body
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

---@class SceneOptions
---@field metersPerPixel number?
---@field gravity Point?

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

---@class BodyShapeGraphicsOptions
---@field position Point?
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?

---@class BodyPhysicsDefinition
---@field linearDamping number?
---@field angularDamping number?
---@field fixedRotation boolean?

---@class BodyShapePhysicsDefinition
---@field density number?
---@field restitution number?
---@field friction number?
---@field isSensor boolean?
---@field isPreSolveEnabled boolean?

---@class BodyOptions
---@field type integer?
---@field shapeKey string?
---@field bodyPhysics BodyPhysicsDefinition?
---@field shapePhysics BodyShapePhysicsDefinition?
---@see sol.BodyType

---@class BodyDefinition
---@field type integer
---@field script string?
---@field physics BodyPhysicsDefinition?
---@field shapes table<string, BodyShapeRectDefinition | BodyShapePolygonDefinition | BodyShapeCircleDefinition | BodyShapeCapsuleDefinition>?
---@see sol.BodyType

---@class BodyShapeDefinitionBase
---@field type integer
---@field physics BodyShapePhysicsDefinition?
---@field graphics table<string, GraphicsPackDefinition>?
---@see sol.BodyShapeType

---@class BodyShapeRectDefinition: BodyShapeDefinitionBase
---@field rect Rectangle

---@class BodyShapePolygonDefinition: BodyShapeDefinitionBase
---@field points Point[]

---@class BodyShapeCircleDefinition: BodyShapeDefinitionBase
---@field center Point
---@field radius number

---@class BodyShapeCapsuleDefinition: BodyShapeDefinitionBase
---@field center1 Point
---@field center2 Point
---@field radius number

---@class GraphicsPackDefinition
---@field position Point?
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?
---@field center Point?
---@field animationIterations integer?
---@field frames GraphicsPackFrameDefinition[]

---@class GraphicsPackFrameDefinition
---@field duration integer? milliseconds
---@field isVisible boolean? default is true
---@field sprites GraphicsPackSpriteDefinition[]

---@class GraphicsPackSpriteDefinition
---@field isVisible boolean? default is true
---@field position Point?
---@field scaleFactor Point?
---@field sprite sol.Sprite | { spriteSheet: sol.SpriteSheet, spriteIndex: integer }

---@class JointDefinition
---@field bodyA integer | sol.Body
---@field bodyB integer | sol.Body
---@field isCollideConnectedEnabled boolean?

---@class DistanceJointDefenition: JointDefinition
---@field isSpringEnbaled boolean?
---@field isMotorEnbaled boolean?
---@field isLimitEnbaled boolean?
---@field localAnchorA Point?
---@field localAnchorB Point?
---@field minLength number?
---@field maxLength number?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorForce number?
---@field motorSpeed number?
---@field length number?

---@class MotorJointDefinition: JointDefinition
---@field linearOffset Point?
---@field angularOffset number?
---@field maxForce number?
---@field maxTorque number?
---@field correctionFactor number?

---@class MouseJointDefinition: JointDefinition
---@field target Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxForce number?

---@class PrismaticJointDefinition: JointDefinition
---@field isSpringEnbaled boolean?
---@field isMotorEnbaled boolean?
---@field isLimitEnbaled boolean?
---@field localAnchorA Point?
---@field localAnchorB Point?
---@field localAxisA Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorForce number?
---@field motorSpeed number?
---@field reference_angle number?
---@field lowerTranslation number?
---@field upperTranslation number?

---@class WeldJointDefinition: JointDefinition
---@field localAnchorA Point?
---@field localAnchorB Point?
---@field reference_angle number?
---@field linearHertz number?
---@field angularHertz number?
---@field linearDampingRatio number?
---@field angularDampingRatio number?

---@class WheelJointDefinition: JointDefinition
---@field isSpringEnbaled boolean?
---@field isMotorEnbaled boolean?
---@field isLimitEnbaled boolean?
---@field localAnchorA Point?
---@field localAnchorB Point?
---@field localAxisA Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorTorque number?
---@field motorSpeed number?
---@field lowerTranslation number?
---@field upperTranslation number?

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

---@class sol.Window
local __window

---@param view sol.View?
function __window:setView(view) end

---@return sol.View?
function __window:getView() end

---@class sol.View
local __view

---@param fragment Fragment
---@return integer
function __view:createFragment(fragment) end

---@param fragment_id integer
---@param fragment Fragment
---@return boolean
function __view:updateFragment(fragment_id, fragment) end

---@param fragment_id integer
---@return Fragment?
function __view:getFragment(fragment_id) end

---@param fragment_id integer
---@return boolean
function __view:deleteFragment(fragment_id) end

---@param fragment_id integer
---@param target? sol.Scene | sol.Form
---@return boolean
function __view:bindFragment(fragment_id, target) end

---@class sol.Scene
local __scene

---@param vector Point
function __scene:setGravity(vector) end

---@param path string
---@return boolean
function __scene:loadTileMap(path) end

---@param id integer
---@return TileMapObject | nil
function __scene:getTileMapObjectById(id) end

---@param name string
---@return TileMapObject | nil
function __scene:getTileMapObjectByName(name) end

---@param class string
---@return TileMapObject[]
function __scene:getTileMapObjectsByClass(class) end

---@param position Point | nil
---@param definition BodyDefinition
---@param script_path string?
---@param script_argument? any
---@return sol.Body
function __scene:createBody(position, definition, script_path, script_argument) end

---@param body integer | sol.Body
---@return boolean
function __scene:destroyBody(body) end

---@param body_id integer
---@return sol.Body | nil
function __scene:getBody(body_id) end

---@param class string
---@param body_options BodyOptions?
function __scene:createBodiesFromMapObjects(class, body_options) end

---@param body integer | sol.Body
---@return boolean
function __scene:setFollowedBody(body) end

function __scene:resetFollowedBody() end

---@alias ContactCallback fun(contact: Contact)
---@alias SensorContactCallback fun(contact: SensorContact)
---@alias PreSolveContactCallback fun(contact: PreSolveContact)
---@alias StepCallback fun(time_passed: integer)

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

---@param callback SensorContactCallback
---@return integer subscription ID
function __scene:subscribeToSensorBeginContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromSensorBeginContact(subscription_id) end

---@param callback SensorContactCallback
---@return integer subscription ID
function __scene:subscribeToSensorEndContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromSesnsorEndContact(subscription_id) end

---@param callback PreSolveContactCallback
---@return integer subscription ID
function __scene:subscribeToPreSolveContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromPreSolveContact(subscription_id) end

---@param callback StepCallback
---@return integer subscription ID
function __scene:subscribeToStep(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromStep(subscription_id) end

---@param definition DistanceJointDefenition
---@return sol.DistanceJoint
function __scene:createDistanceJoint(definition) end

---@param definition MotorJointDefinition
---@return sol.MotorJoin
function __scene:createMotorJoint(definition) end

---@param definition MouseJointDefinition
---@return integer
function __scene:createMouseJoint(definition) end

---@param definition PrismaticJointDefinition
---@return sol.PrismaticJoint
function __scene:createPrismaticJoint(definition) end

---@param definition WeldJointDefinition
---@return sol.WeldJoint
function __scene:createWeldJoint(definition) end

---@param definition WheelJointDefinition
---@return sol.WheelJoint
function __scene:createWheelJoint(definition) end

---@param id integer
---@return sol.DistanceJoint | nil
function __scene:getDistanceJoint(id) end

---@param id integer
---@return sol.MotorJoin | nil
function __scene:getMotorJoint(id) end

---@param id integer
---@return sol.MotorJoin | nil
function __scene:getMouseJoint(id) end

---@param id integer
---@return sol.PrismaticJoint | nil
function __scene:getPrismaticJoint(id) end

---@param id integer
---@return sol.WeldJoint | nil
function __scene:getWeldJoint(id) end

---@param id integer
---@return sol.WheelJoint | nil
function __scene:getWheelJoint(id) end

---@param joint sol.Joint | integer
---@return boolean
function __scene:destroyJoint(joint) end

---@param body_id integer | sol.Body
---@param destination Point
---@return Point[] | nil
function __scene:findPath(body_id, destination) end

---@class sol.Body
local __body

---@return boolean
function __body:isValid() end

---@return integer
function __body:getId() end

---@param shpe_key string
---@return sol.BodyShape | nil
function __body:getShape(shpe_key) end

---@param layer string
function __body:setLayer(layer) end

--- Throws an error if the body is invalid or has been destroyed
---@return Point
function __body:getPosition() end

---@pram position Point
function __body:setPosition(position) end

---@pram vector Point
function __body:applyForceToCenter(vector) end

---@pram vector Point
function __body:applyImpulseToCenter(vector) end

--- Throws an error if the body is invalid or has been destroyed
---@return Point
function __body:getLinearVelocity() end

---@pram velocity Point
function __body:setLinearVelocity(velocity) end

--- Throws an error if the body is invalid or has been destroyed
---@return number
function __body:getMass() end

---@class sol.BodyShape
local __body_shape

---@return boolean
function __body_shape:isValid() end

---@return string
function __body_shape:getKey() end

---@return sol.Body
function __body_shape:getBody() end

---@param graphic_key string
---@return sol.GraphicsPack | nil
function __body_shape:getGraphicsPack(graphic_key) end

---@param graphic_key string
---@return boolean
function __body_shape:setCurrentGraphics(graphic_key) end

---@param graphic_key string
---@param flip_horizontally boolean
---@param flip_vertically boolean
---@return boolean
function __body_shape:flipGraphics(graphic_key, flip_horizontally, flip_vertically) end

---@class sol.Joint
local __joint

---@return boolean
function __joint:isValid() end

---@return integer
function __joint:getId() end

---@return sol.Body
function __joint:getBodyA() end

---@return sol.Body
function __joint:getBodyB() end

---@return Point
function __joint:getLocalAnchorA() end

---@return Point
function __joint:getLocalAnchorB() end

---@return boolean
function __joint:isCollideConnectedEnabled() end

---@param enable boolean
function __joint:enableCollideConnected(enable) end

---@class sol.DistanceJoint: sol.Joint
local __distance_joint

---@return number
function __distance_joint:getLength() end

---@param length number
function __distance_joint:setLength(length) end

---@return boolean
function __distance_joint:isSpringEnabled() end

---@param enable boolean
function __distance_joint:enableSpring(enable) end

---@return number
function __distance_joint:getSpringDampingRatio() end

---@param ratio number
function __distance_joint:setSpringDampingRatio(ratio) end

---@return number
function __distance_joint:getSpringHertz() end

---@param hertz number
function __distance_joint:setSpringHertz(hertz) end

---@return boolean
function __distance_joint:isLimtEnabled() end

---@param enable boolean
function __distance_joint:enableLimit(enable) end

---@return number
function __distance_joint:getMinLength() end

---@return number
function __distance_joint:getMaxLength() end

---@return number
function __distance_joint:getCurrentLength() end

---@param min_length number
---@param max_length number
function __distance_joint:setLengthRange(min_length, max_length) end

---@return boolean
function __distance_joint:isMotorEnabled() end

---@param enable boolean
function __distance_joint:enableMotor(enable) end

---@return number
function __distance_joint:getMotorSpeed() end

---@param speed number
function __distance_joint:setMotorSpeed(speed) end

---@return number
function __distance_joint:getMotorForce() end

---@return number
function __distance_joint:getMaxMotorForce() end

---@param force number
function __distance_joint:setMaxMotorForce(force) end

---@class sol.MotorJoin: sol.Joint
local __motor_joint

---@return Point
function getLinearOffset() end

---@return Point
function __motor_joint:setLinearOffset() end

---@return number
function __motor_joint:getAngularOffset() end

---@param offset number
function __motor_joint:setAngularOffset(offset) end

---@return number
function __motor_joint:getMaxForce() end

---@param force number
function __motor_joint:setMaxForce(force) end

---@return number
function __motor_joint:getMaxTorque() end

---@param torque number
function __motor_joint:setMaxTorque(torque) end

---@return number
function __motor_joint:getCorrectionFactor() end

---@param correction_factor number
function __motor_joint:setCorrectionFactor(correction_factor) end

---@class sol.MouseJoint: sol.Joint
local __mouse_joint

---@return Point
function __mouse_joint:getTarget() end

---@param target Point
function __mouse_joint:setTarget(target) end

---@return number
function __mouse_joint:getSpringHertz() end

---@param hertz number
function __mouse_joint:setSpringHertz(hertz) end

---@return number
function __mouse_joint:getSpringDampingRatio() end

---@param ratio number
function __mouse_joint:setSpringDampingRatio(ratio) end

---@return number
function __mouse_joint:getMaxForce() end

---@param force number
function __mouse_joint:setMaxForce(force) end

---@class sol.PrismaticJoint: sol.Joint
local __prismatic_joint

---@return boolean
function __prismatic_joint:isSpringEnabled() end

---@param enable boolean
function __prismatic_joint:enableSpring(enable) end

---@return number
function __prismatic_joint:getSpringHertz() end

---@param hertz number
function __prismatic_joint:setSpringHertz(hertz) end

---@return number
function __prismatic_joint:getSpringDampingRatio() end

---@param ratio number
function __prismatic_joint:setSpringDampingRatio(ratio) end

---@return boolean
function __prismatic_joint:isLimitEnabled() end

---@param enable boolean
function __prismatic_joint:enableLimit(enable) end

---@return number
function __prismatic_joint:getLowerLimit() end

---@return number
function __prismatic_joint:getUpperLimit() end

---@param lower_limit number
---@param upper_limit number
function __prismatic_joint:setLimits(lower_limit, upper_limit) end

---@return boolean
function __prismatic_joint:isMotorEnabled() end

---@param enable boolean
function __prismatic_joint:enableMotor(enable) end

---@return number
function __prismatic_joint:getMotorSpeed() end

---@param speed number
function __prismatic_joint:setMotorSpeed(speed) end

---@return number
function __prismatic_joint:getMaxMotorForce() end

---@return number
function __prismatic_joint:getMotorForce() end

---@param force number
function __prismatic_joint:setMaxMotorForce(force) end

---@class sol.RevoluteJoint: sol.Joint
local __revolute_joint

---@return boolean
function __revolute_joint:isSpringEnabled() end

---@param enable boolean
function __revolute_joint:enableSpring(enable) end

---@return number
function __revolute_joint:getSpringHertz() end

---@param hertz number
function __revolute_joint:setSpringHertz(hertz) end

---@return number
function __revolute_joint:getSpringDampingRatio() end

---@param ratio number
function __revolute_joint:setSpringDampingRatio(ratio) end

---@return number
function __revolute_joint:getAngle() end

---@return boolean
function __revolute_joint:isLimitEnabled() end

---@param enable boolean
function __revolute_joint:enableLimit(enable) end

---@param lower_limit number
---@param upper_limit number
function __revolute_joint:setLimits(lower_limit, upper_limit) end

---@return number
function __revolute_joint:getLowerLimit() end

---@return number
function __revolute_joint:getUpperLimit() end

---@return boolean
function __revolute_joint:isMotorEnabled() end

---@param enable boolean
function __revolute_joint:enableMotor(enable) end

---@return number
function __revolute_joint:getMotorSpeed() end

---@param speed number
function __revolute_joint:setMotorSpeed(speed) end

---@return number
function __revolute_joint:getMotorTorque() end

---@return number
function __revolute_joint:getMaxMotorTorque() end

---@param torque number
function __revolute_joint:setMaxMotorTorque(torque) end

---@class sol.WeldJoint: sol.Joint
local __weld_joint

---@return number
function __weld_joint:getReferenceAngle() end

---@param angle number
function __weld_joint:setReferenceAngle(angle) end

---@return number
function __weld_joint:getLinearHertz() end

---@param hertz number
function __weld_joint:setLinearHertz(hertz) end

---@return number
function __weld_joint:getLinearDampingRatio() end

---@param ratio number
function __weld_joint:setLinearDampingRatio(ratio) end

---@return number
function __weld_joint:getAngularHertz() end

---@param hertz number
function __weld_joint:setAngularHertz(hertz) end

---@return number
function __weld_joint:getAngularDampingRatio() end

---@param ratio number
function __weld_joint:setAngularDampingRatio(ratio) end

---@class sol.WheelJoint: sol.Joint
local __wheel_joint

---@return boolean
function __wheel_joint:isSpringEnabled() end

---@param enable boolean
function __wheel_joint:enableSpring(enable) end

---@return number
function __wheel_joint:getSpringHertz() end

---@param hertz number
function __wheel_joint:setSpringHertz(hertz) end

---@return number
function __wheel_joint:getSpringDampingRatio() end

---@param ratio number
function __wheel_joint:setSpringDampingRatio(ratio) end

---@return boolean
function __wheel_joint:isLimitEnabled() end

---@return number
function __wheel_joint:getLowerLimit() end

---@return number
function __wheel_joint:getUpperLimit() end

---@param lower_limit number
---@param upper_limit number
function __wheel_joint:setLimits(lower_limit, upper_limit) end

---@param enable boolean
function __wheel_joint:enableLimit(enable) end

---@return boolean
function __wheel_joint:isMotorEnabled() end

---@param enable boolean
function __wheel_joint:enableMotor(enable) end

---@return number
function __wheel_joint:getMotorSpeed() end

---@param speed number
function __wheel_joint:setMotorSpeed(speed) end

---@return number
function __wheel_joint:getMaxMotorTorque() end

---@param torque number
function __wheel_joint:setMaxMotorTorque(torque) end

---@return number
function __wheel_joint:getMotorTorque() end

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

---@class SensorContact
---@field sensor ContactSide
---@field visitor ContactSide

---@class PreSolveContact
---@field sideA ContactSide
---@field sideB ContactSide
---@field manifold Manifold

---@class Manifold
---@field normal Point
---@field points ManifoldPoint[]

---@class ManifoldPoint
---@field point Point
---@field anchorA Point
---@field anchorB Point
---@field separation number
---@field normalImpulse number
---@field tangentImpulse number
---@field normalVelocity number

---@class ContactSide
---@field bodyId integer
---@field shapeKey string
---@field tileMapObjectId integer?

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
---@param options SceneOptions?
---@return sol.Scene
function __store:createScene(key, options) end

---@param key string
---@return sol.Scene | nil
function __store:getScene(key) end

---@param key string
---@return boolean
function __store:freeScene(key) end

---@param key string
---@return sol.Form
function __store:createForm(key) end

---@param key string
---@return sol.Form | nil
function __store:getForm(key) end

---@param key string
---@return boolean
function __store:freeForm(key) end

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

---@class sol.BodyShapeType
---@field CIRCLE integerw
---@field POLYGON integer
---@field CAPSULE integer

---@class sol.Sprite
local __sprite

---@param path string
---@param options SpriteOptions?
---@return boolean
function __sprite:loadFromFile(path, options) end

---@return boolean
function __sprite:isValid() end

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
---@param options SpriteSheetOptions?
---@return boolean
function __sprite_sheet:loadFromFile(path, options) end

---@class sol.GraphicsPack
local __graphics_pack

---@param definition GraphicsPackFrameDefinition
---@return integer # index of the new frame
function __graphics_pack:addFrame(definition) end

---@param frame_index integer # position to insert
---@param definition GraphicsPackFrameDefinition
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
function __graphics_pack:switchToNextVisibleFrame() end

---@param frame_index integer
---@param definition? GraphicsPackSpriteDefinition
function __graphics_pack:addSprite(frame_index, definition) end

---@param frame_index integer
---@param sprite_index integer
---@return boolean
function __graphics_pack:removeSprite(frame_index, sprite_index) end

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
