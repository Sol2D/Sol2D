local keys = require 'resources.keys'

local player_id = self.bodyId
local scene = self.scene

---@type BodyContactObserver
local contact_observer = self.arg.contactObserver

local player_mass = scene:getBodyMass(player_id)

local WALK_VELOCITY = 5
local JUMP_DELAY = 40

local Direction = {
    LEFT = 0,
    RIGHT = 1
}

local Action = {
    IDLE = 0,
    WALK = 1
}

local state = {
    inAir = false,
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
        end
        self.action = action
        self.direction = direction
        scene:setBodyShapeCurrentGraphic(player_id, keys.shapes.player.main, graphic)
    end
end

local function getForce(current_velocity, desired_velocity)
    local change = desired_velocity - current_velocity
    return player_mass * change / (1 / 60) -- TODO: frame rate
end

sol.heartbeat:subscribe(function()
    local right_key, left_key, space_key = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE
    )

    if not state.inAir then
        if state.jumpTimeout > 0 then
            state.jumpTimeout = state.jumpTimeout - 1
        elseif space_key then
            state.jumpTimeout = JUMP_DELAY
            scene:applyImpulseToBodyCenter(player_id, { x = 0, y = -1300 })
        end
    end

    if right_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getForce(scene:getBodyLinearVelocity(player_id).x, WALK_VELOCITY),
                y = 0
            }
        )
        state:setAction(Direction.RIGHT, Action.WALK)
    elseif left_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getForce(scene:getBodyLinearVelocity(player_id).x, -WALK_VELOCITY),
                y = 0
            }
        )
        state:setAction(Direction.LEFT, Action.WALK)
    else
        state:setAction(state.direction, Action.IDLE)
    end
end)

contact_observer.setSensorBeginContactListener(player_id, function (sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == keys.shapes.player.bottomSensor then
        state.inAir = false
    end
end)

contact_observer.setSensorEndContactListener(player_id, function(sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == keys.shapes.player.bottomSensor then
        state.inAir = true
    end
end)
