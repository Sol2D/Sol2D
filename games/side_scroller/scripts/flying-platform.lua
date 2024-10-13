local scene = script.scene

if #script.arg.points < 2 then
    print('Point count < 2, platform is static. Count:', #script.arg.points)
    return
end

local platform = script.body

local DELTA = 0.05
local VELOCITY = 2

local function makePlaces()
    local points = script.arg.points
    local places = {}
    local prev_point = points[1]
    for i = 2, #points do
        local delta_x = points[i].x - prev_point.x
        local delta_y = points[i].y - prev_point.y
        local length = math.sqrt(delta_x ^ 2 + delta_y ^ 2)
        local d = math.abs(length) / VELOCITY
        table.insert(places, {
            source = prev_point,
            destination = points[i],
            velocity = {
                x = delta_x / d,
                y = delta_y / d
            }
        })
        prev_point = points[i]
    end
    local count = #places
    for i = count, 1, -1 do
        table.insert(places, {
            source = places[i].destination,
            destination = places[i].source,
            velocity = {
                x = -places[i].velocity.x,
                y = -places[i].velocity.y
            }
        })
    end
    return places
end

local places = makePlaces()
local target_index = 0

---@param point1 Point
---@param point2 Point
---@return boolean
local function areSamePoints(point1, point2)
    return math.abs(point1.x - point2.x) <= DELTA and math.abs(point1.y - point2.y) <= DELTA
end

scene:subscribeToStep( -- FIXME: this subscription must die with the object
    function()
        local position = platform:getPosition()
        if not position then
            print('Platform position is nil')
            return
        end
        if target_index == 0 or areSamePoints(position, places[target_index].destination) then
            target_index = target_index + 1
            if target_index > #places or target_index < 1 then
                target_index = 1
            end
            platform:setLinearVelocity(
                places[target_index].velocity
            )
        end
    end
)
