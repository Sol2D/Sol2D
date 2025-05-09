---@meta

---@class sol.Mouse
local __mouse

---@vararg integer
---@return sol.MouseState
function __mouse:getState() end

---@class sol.MouseState
---@field left boolean
---@field right boolean
---@field middle boolean
---@field x1 boolean
---@field x2 boolean
---@field point sol.Point
