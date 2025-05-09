---@meta

---@class sol.JointDefinition
---@field bodyA integer | sol.Body
---@field bodyB integer | sol.Body
---@field isCollideConnectedEnabled boolean?

---@class sol.DistanceJointDefinition: sol.JointDefinition
---@field isSpringEnabled boolean?
---@field isMotorEnabled boolean?
---@field isLimitEnabled boolean?
---@field localAnchorA sol.Point?
---@field localAnchorB sol.Point?
---@field minLength number?
---@field maxLength number?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorForce number?
---@field motorSpeed number?
---@field length number?

---@class sol.MotorJointDefinition: sol.JointDefinition
---@field linearOffset sol.Point?
---@field angularOffset number?
---@field maxForce number?
---@field maxTorque number?
---@field correctionFactor number?

---@class sol.MouseJointDefinition: sol.JointDefinition
---@field target sol.Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxForce number?

---@class sol.PrismaticJointDefinition: sol.JointDefinition
---@field isSpringEnabled boolean?
---@field isMotorEnabled boolean?
---@field isLimitEnabled boolean?
---@field localAnchorA sol.Point?
---@field localAnchorB sol.Point?
---@field localAxisA sol.Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorForce number?
---@field motorSpeed number?
---@field reference_angle number?
---@field lowerTranslation number?
---@field upperTranslation number?

---@class sol.WeldJointDefinition: sol.JointDefinition
---@field localAnchorA sol.Point?
---@field localAnchorB sol.Point?
---@field reference_angle number?
---@field linearHertz number?
---@field angularHertz number?
---@field linearDampingRatio number?
---@field angularDampingRatio number?

---@class sol.WheelJointDefinition: sol.JointDefinition
---@field isSpringEnabled boolean?
---@field isMotorEnabled boolean?
---@field isLimitEnabled boolean?
---@field localAnchorA sol.Point?
---@field localAnchorB sol.Point?
---@field localAxisA sol.Point?
---@field hertz number?
---@field dampingRatio number?
---@field maxMotorTorque number?
---@field motorSpeed number?
---@field lowerTranslation number?
---@field upperTranslation number?

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

---@return sol.Point
function __joint:getLocalAnchorA() end

---@return sol.Point
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
function __distance_joint:isLimitEnabled() end

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

---@class sol.MotorJoint: sol.Joint
local __motor_joint

---@return sol.Point
function getLinearOffset() end

---@return sol.Point
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

---@return sol.Point
function __mouse_joint:getTarget() end

---@param target sol.Point
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
