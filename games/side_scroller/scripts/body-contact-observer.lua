local module_meta = {}
module_meta.__index = module_meta

---@param storage table<integer, fun(body: sol.ContactSide, other: sol.ContactSide)>
---@param contact sol.Contact
local function call(storage, contact)
    local callback = storage[contact.sideA.bodyId]
    if callback then
        callback(contact.sideA, contact.sideB)
    end
    callback = storage[contact.sideB.bodyId]
    if callback then
        callback(contact.sideB, contact.sideA)
    end
end

---@param storage table<integer, fun(sesnsor: sol.ContactSide, visitor: sol.ContactSide)>
---@param contact sol.SensorContact
local function callSensor(storage, contact)
    local callback = storage[contact.sensor.bodyId]
    if callback then
        callback(contact.sensor, contact.visitor)
    end
    callback = storage[contact.visitor.bodyId]
    if callback then
        callback(contact.sensor, contact.visitor)
    end
end

---@return BodyContactObserver
function module_meta.new()
    local beginContactCallbackStorage = {}
    local endContactCallbackStorage = {}
    local beginSensorContactCallbackStorage = {}
    local endSensorContactCallbackStorage = {}
    ---@class BodyContactObserver
    local observer = {
        ---@param body_id integer
        ---@param callback fun(body: sol.ContactSide, other: sol.ContactSide)
        setBeginContactListener = function(body_id, callback)
            beginContactCallbackStorage[body_id] = callback
        end,
        ---@param body_id integer
        removeBeginContactListener = function(body_id)
            beginContactCallbackStorage[body_id] = nil
        end,
        ---@param contact sol.Contact
        callBeginContact = function(contact)
            call(beginContactCallbackStorage, contact)
        end,

        ---@param body_id integer
        ---@param callback fun(body: sol.ContactSide, other: sol.ContactSide)
        setEndContactListener = function(body_id, callback)
            endContactCallbackStorage[body_id] = callback
        end,
        ---@param body_id integer
        removeEndContactListener = function(body_id)
            endContactCallbackStorage[body_id] = nil
        end,
        ---@param contact sol.Contact
        callEndContact = function(contact)
            call(endContactCallbackStorage, contact)
        end,

        ---@param body_id  integer
        ---@param callback fun(sesnsor: sol.ContactSide, visitor: sol.ContactSide)
        setSensorBeginContactListener = function(body_id, callback)
            beginSensorContactCallbackStorage[body_id] = callback
        end,
        ---@param body_id  integer
        removeSensorBeginContactListener = function(body_id)
            beginSensorContactCallbackStorage[body_id] = nil
        end,
        ---@param contact sol.SensorContact
        callSensorBeginContact = function(contact)
            callSensor(beginSensorContactCallbackStorage, contact)
        end,

        ---@param body_id  integer
        ---@param callback fun(sesnsor: sol.ContactSide, visitor: sol.ContactSide)
        setSensorEndContactListener = function(body_id, callback)
            endSensorContactCallbackStorage[body_id] = callback
        end,
        ---@param body_id  integer
        removeSensorEndContactListener = function(body_id)
            endSensorContactCallbackStorage[body_id] = nil
        end,
        ---@param contact sol.SensorContact
        callSensorEndContact = function(contact)
            callSensor(endSensorContactCallbackStorage, contact)
        end
    }
    return observer
end

return setmetatable({}, module_meta)
