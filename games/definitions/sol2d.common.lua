---@meta

---@class sol.Point
---@field x number
---@field y number

---@class sol.Color
---@field r integer
---@field g integer
---@field b integer
---@field a integer?

---@class sol.Size
---@field w number
---@field h number

---@class sol.Rectangle
---@field x number
---@field y number
---@field w number
---@field h number

---@alias sol.Rotation number | { cosine: number, sine: number }

---@class sol.Transform
---@field translation sol.Point?
---@field rotation sol.Rotation?
