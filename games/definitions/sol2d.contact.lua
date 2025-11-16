---@meta

---@class sol.Contact
---@field sideA sol.ContactSide
---@field sideB sol.ContactSide

---@class sol.SensorContact
---@field sensor sol.ContactSide
---@field visitor sol.ContactSide

---@class sol.PreSolveContact
---@field sideA sol.ContactSide
---@field sideB sol.ContactSide
---@field point sol.Point
---@field normal sol.Point

---@class sol.ContactSide
---@field bodyId integer
---@field shapeKey string
---@field tileMapObjectId integer?
