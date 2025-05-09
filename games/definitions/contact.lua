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
---@field manifold sol.Manifold

---@class sol.Manifold
---@field normal sol.Point
---@field points sol.ManifoldPoint[]

---@class sol.ManifoldPoint
---@field point sol.Point
---@field anchorA sol.Point
---@field anchorB sol.Point
---@field separation number
---@field normalImpulse number
---@field tangentImpulse number
---@field normalVelocity number

---@class sol.ContactSide
---@field bodyId integer
---@field shapeKey string
---@field tileMapObjectId integer?
