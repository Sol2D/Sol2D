local keys = require 'resources.keys'

local player_id = self.bodyId
local scene = self.scene

---@type BodyContactObserver
local contact_observer = self.arg.contactObserver

local player_mass = scene:getBodyMass(player_id)

local WALK_VELOCITY = 5
local JUMP_DELAY = 15

local Direction = {
    LEFT = 0,
    RIGHT = 1
}

local Action = {
    IDLE = 0,
    WALK = 1,
    JUMP = 2
}

local state = {
    inAir = false,
    footings = {},
    jumpTimeout = 0,
    direction = Direction.LEFT,
    action = Action.IDLE
}

function state:setAction(direction, action)
    if direction ~= self.direction or action ~= self.action then
        local graphic
        if action == Action.IDLE then
            if direction == Direction.LEFT then
                graphic = keys.shapeGraphics.player.idleLeft
            else
                graphic = keys.shapeGraphics.player.idleRight
            end
        elseif action == Action.WALK then
            if direction == Direction.LEFT then
                graphic = keys.shapeGraphics.player.walkLeft
            else
                graphic = keys.shapeGraphics.player.walkRight
            end
        elseif action == Action.JUMP then
            if direction == Direction.LEFT then
                graphic = keys.shapeGraphics.player.jumpLeft
            else
                graphic = keys.shapeGraphics.player.jumpRight
            end
        end
        self.action = action
        self.direction = direction
        scene:setBodyShapeCurrentGraphics(player_id, keys.shapes.player.main, graphic)
    end
end

local function getFootingVelocity()
    if #state.footings > 0 then
        return scene:getBodyLinearVelocity(state.footings[1].bodyId)
    end
    return { x = 0, y = 0 }
end

local function getHorizontalForce(current_velocity, footing_velocity, desired_velocity)
    local change = desired_velocity - (current_velocity - footing_velocity)
    return player_mass * change / (1 / 60) -- TODO: frame rate
end

sol.heartbeat:subscribe(function()
    local right_key, left_key, space_key = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE
    )

    local action = Action.IDLE
    local direction = state.direction

    if state.inAir then
        action = Action.JUMP
    else
        if state.jumpTimeout > 0 then
            state.jumpTimeout = state.jumpTimeout - 1
        elseif space_key then
            state.jumpTimeout = JUMP_DELAY
            scene:applyImpulseToBodyCenter(player_id, { x = 0, y = -1300 })
            action = Action.JUMP
        end
    end

    if right_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getHorizontalForce(
                    scene:getBodyLinearVelocity(player_id).x,
                    getFootingVelocity().x,
                    WALK_VELOCITY
                ),
                y = 0
            }
        )
        direction = Direction.RIGHT
        if not state.inAir then
            action = Action.WALK
        end
    elseif left_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getHorizontalForce(
                    scene:getBodyLinearVelocity(player_id).x,
                    getFootingVelocity().x,
                    -WALK_VELOCITY
                ),
                y = 0
            }
        )
        direction = Direction.LEFT
        if not state.inAir then
            action = Action.WALK
        end
    end

    state:setAction(direction, action)
end)

contact_observer.setSensorBeginContactListener(player_id, function (sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == keys.shapes.player.bottomSensor then
        table.insert(state.footings, visitor)
        state.inAir = false
    end
end)

contact_observer.setSensorEndContactListener(player_id, function(sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == keys.shapes.player.bottomSensor then
        for index, footing in ipairs(state.footings) do
            if footing.bodyId == visitor.bodyId and footing.shapeKey == visitor.shapeKey then
                table.remove(state.footings, index)
                break
            end
        end
        state.inAir = #state.footings == 0
    end
end)
