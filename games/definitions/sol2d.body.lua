---@meta

---@class sol.BodyOptions
---@field type integer?
---@field shapeKey string?
---@field bodyPhysics sol.BodyPhysicsDefinition?
---@field shapePhysics sol.BodyShapePhysicsDefinition?
---@see sol.BodyType

---@class sol.BodyShapeGraphicsOptions
---@field position sol.Point?
---@field isFlippedHorizontally boolean?
---@field isFlippedVertically boolean?

---@class sol.BodyPhysicsDefinition
---@field linearDamping number?
---@field angularDamping number?
---@field fixedRotation boolean?

---@class sol.BodyShapePhysicsDefinition
---@field density number?
---@field restitution number?
---@field friction number?
---@field isSensor boolean?
---@field isPreSolveEnabled boolean?

---@class sol.BodyDefinition
---@field type integer
---@field script string?
---@field physics sol.BodyPhysicsDefinition?
---@field shapes table<string, sol.BodyShapeRectDefinition | sol.BodyShapePolygonDefinition | sol.BodyShapeCircleDefinition | sol.BodyShapeCapsuleDefinition>?
---@see sol.BodyType

---@class sol.BodyShapeDefinitionBase
---@field type integer
---@field physics sol.BodyShapePhysicsDefinition?
---@field graphics table<string, sol.GraphicsPackDefinition>?
---@see sol.BodyShapeType

---@class sol.BodyShapeRectDefinition: sol.BodyShapeDefinitionBase
---@field rect sol.Rectangle

---@class sol.BodyShapePolygonDefinition: sol.BodyShapeDefinitionBase
---@field points sol.Point[]

---@class sol.BodyShapeCircleDefinition: sol.BodyShapeDefinitionBase
---@field center sol.Point
---@field radius number

---@class sol.BodyShapeCapsuleDefinition: sol.BodyShapeDefinitionBase
---@field center1 sol.Point
---@field center2 sol.Point
---@field radius number

---@class sol.BodyType
---@field STATIC integer
---@field DYNAMIC integer
---@field KINEMATIC integer

---@class sol.BodyShapeType
---@field CIRCLE integer
---@field POLYGON integer
---@field CAPSULE integer

---@class sol.Body
local __body

---@return boolean
function __body:isValid() end

---@return integer
function __body:getId() end

---@param shape_key string
---@return sol.BodyShape | nil
function __body:getShape(shape_key) end

---@param layer string
function __body:setLayer(layer) end

--- Throws an error if the body is invalid or has been destroyed
---@return sol.Point
function __body:getPosition() end

---@pram position sol.Point
function __body:setPosition(position) end

---@pram vector sol.Point
function __body:applyForceToCenter(vector) end

---@pram vector sol.Point
function __body:applyImpulseToCenter(vector) end

--- Throws an error if the body is invalid or has been destroyed
---@return sol.Point
function __body:getLinearVelocity() end

---@pram velocity sol.Point
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

---@return sol.GraphicsPack | nil
function __body_shape:getCurrentGraphicsPack() end

---@param graphic_key string
---@return boolean
function __body_shape:setCurrentGraphics(graphic_key) end

---@param graphic_key string
---@param flip_horizontally boolean
---@param flip_vertically boolean
---@return boolean
function __body_shape:flipGraphics(graphic_key, flip_horizontally, flip_vertically) end
