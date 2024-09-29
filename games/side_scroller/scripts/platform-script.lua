if #self.arg.points < 2 then
    print('Point count < 2, platform is static. Count:', #self.arg.points)
    return
end

local platform_id = self.bodyId
local scene = self.scene

local DELTA = 0.01
local VELOCITY = 3

local function makePlaces()
    local places = {}
    local prev_point = self.arg.points[1]
    for i = 2, #self.arg.points do
        local delta_x = self.arg.points[i].x - prev_point.x
        local delta_y = self.arg.points[i].y - prev_point.y
        local length = math.sqrt(delta_x ^ 2 + delta_y ^ 2)
        local d = math.abs(length) / VELOCITY
        table.insert(places,{
            source = prev_point,
            destination = self.arg.points[i],
            velocity = {
                x = delta_x / d,
                y = delta_y / d
            }
        })
        prev_point = self.arg.points[i]
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

sol.heartbeat:subscribe( -- FIXME: this subscription must die with the object
    function ()
        local position = scene:getBodyPosition(platform_id)
        if position == nil then
            print('Platform position is nil')
            return
        end
        if target_index == 0 or areSamePoints(position, places[target_index].destination) then
            target_index = target_index + 1
            if target_index > #places or target_index < 1 then
                target_index = 1
            end
            scene:setBodyLinearVelocity(
                platform_id,
                places[target_index].velocity
            )
        end
    end
)
