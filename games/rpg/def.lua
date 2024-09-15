local meters_per_pixel = 0.05

Def = {}
Def.metersPerPixel = meters_per_pixel

---@param point Point
---@return Point
function Def.pixelPointToPhisical(point)
    return { x = point.x * meters_per_pixel, y = point.y * meters_per_pixel }
end

return Def
