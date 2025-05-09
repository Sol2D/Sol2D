---@meta

---@class sol.SceneOptions
---@field metersPerPixel number?
---@field gravity sol.Point?

---@class sol.Scene
local __scene

---@param color sol.Color
function __scene:setBackgroundColor(color) end

---@param vector sol.Point
function __scene:setGravity(vector) end

---@param path string
---@return boolean
function __scene:loadTileMap(path) end

---@param id integer
---@return sol.TileMapObject | nil
function __scene:getTileMapObjectById(id) end

---@param name string
---@return sol.TileMapObject | nil
function __scene:getTileMapObjectByName(name) end

---@param class string
---@return sol.TileMapObject[]
function __scene:getTileMapObjectsByClass(class) end

---@param position sol.Point | nil
---@param definition sol.BodyDefinition
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
---@param body_options sol.BodyOptions?
function __scene:createBodiesFromMapObjects(class, body_options) end

---@param body integer | sol.Body
---@return boolean
function __scene:setFollowedBody(body) end

function __scene:resetFollowedBody() end

---@alias sol.ContactCallback fun(contact: sol.Contact)
---@alias sol.SensorContactCallback fun(contact: sol.SensorContact)
---@alias sol.PreSolveContactCallback fun(contact: sol.PreSolveContact)
---@alias sol.StepCallback fun(time_passed: integer)

---@param callback sol.ContactCallback
---@return integer subscription ID
function __scene:subscribeToBeginContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromBeginContact(subscription_id) end

---@param callback sol.ContactCallback
---@return integer subscription ID
function __scene:subscribeToEndContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromEndContact(subscription_id) end

---@param callback sol.SensorContactCallback
---@return integer subscription ID
function __scene:subscribeToSensorBeginContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromSensorBeginContact(subscription_id) end

---@param callback sol.SensorContactCallback
---@return integer subscription ID
function __scene:subscribeToSensorEndContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromSensorEndContact(subscription_id) end

---@param callback sol.PreSolveContactCallback
---@return integer subscription ID
function __scene:subscribeToPreSolveContact(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromPreSolveContact(subscription_id) end

---@param callback sol.StepCallback
---@return integer subscription ID
function __scene:subscribeToStep(callback) end

---@param subscription_id integer
function __scene:unsubscribeFromStep(subscription_id) end

---@param definition sol.DistanceJointDefinition
---@return sol.DistanceJoint
function __scene:createDistanceJoint(definition) end

---@param definition sol.MotorJointDefinition
---@return sol.MotorJoint
function __scene:createMotorJoint(definition) end

---@param definition sol.MouseJointDefinition
---@return integer
function __scene:createMouseJoint(definition) end

---@param definition sol.PrismaticJointDefinition
---@return sol.PrismaticJoint
function __scene:createPrismaticJoint(definition) end

---@param definition sol.WeldJointDefinition
---@return sol.WeldJoint
function __scene:createWeldJoint(definition) end

---@param definition sol.WheelJointDefinition
---@return sol.WheelJoint
function __scene:createWheelJoint(definition) end

---@param id integer
---@return sol.DistanceJoint | nil
function __scene:getDistanceJoint(id) end

---@param id integer
---@return sol.MotorJoint | nil
function __scene:getMotorJoint(id) end

---@param id integer
---@return sol.MotorJoint | nil
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
---@param destination sol.Point
---@return sol.Point[] | nil
function __scene:findPath(body_id, destination) end
