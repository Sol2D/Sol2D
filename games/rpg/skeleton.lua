require 'def'

local body_id = self.bodyId
local scene = self.scene

local DELTA = 5

local skeleton_track = scene:getTileMapObjectByName(self.arg.trackName)
if skeleton_track == nil then
    print('The skeleton track not found')
    return
else
    print('Using the skeleton track:', self.arg.trackName)
end

local points = skeleton_track.points

if points == nil then
    print('The skeleton track does not contain point')
    return
end

for i = 1, #points, 1 do
    points[i].x = points[i].x + skeleton_track.position.x
    points[i].y = points[i].y + skeleton_track.position.y
end

scene:setBodyPosition(body_id, Def.pixelPointToPhisical(points[self.arg.startPoint]))

local path = scene:findPath(body_id, points[3])
if path == nil then
    print('Path not found');
    return
end
if #path == 1 then
    print('Path contains the only one point');
    return
end

for i = 1, #path do
    print(path[i].x, path[i].y)
end

local is_forward_direction = true
local next_point_idx = 1

local function getNextPoint()
    return path[next_point_idx]
end

local function incrementPoint()
    if is_forward_direction then
        next_point_idx = next_point_idx + 1
        if next_point_idx > #path then
            is_forward_direction = false
            next_point_idx = next_point_idx - 2
        end
    else
        next_point_idx = next_point_idx - 1
        if next_point_idx < 1 then
            is_forward_direction = true
            next_point_idx = 2
        end
    end
end

local function areSamePoints(point1, point2)
    return math.abs(point1.x - point2.x) <= DELTA and math.abs(point1.y - point2.y) <= DELTA
end

local function getForce(position, destination)
    local delta_x = destination.x - position.x
    local delta_y = destination.y - position.y
    if math.abs(delta_x) > math.abs(delta_y) then
        if delta_x > 0 then
            return { x = 2000, y = 0 }
        else
            return { x = -2000, y = 0 }
        end
    else
        if delta_y > 0 then
            return { x = 0, y = 2000 }
        else
            return { x = 0, y = -2000 }
        end
    end
end

local function getGraphics(force)
    if force.x == 0 then
        if force.y > 0 then
            return 'down'
        else
            return 'up'
        end
    else
        if force.x > 0 then
            return 'right'
        else
            return 'left'
        end
    end
end

-- TODO: how to unsubscribe when the body is dead?
sol.heartbeat:subscribe(function ()
    local next_point = getNextPoint()
    local position = scene:getBodyPosition(body_id)
    if areSamePoints(position, next_point) then
        incrementPoint()
        next_point = getNextPoint()
    end
    local force = getForce(position, next_point)
    scene:setBodyShapeCurrentGraphics(body_id, 'main', getGraphics(force))
    scene:applyForceToBodyCenter(body_id, force)
end)
