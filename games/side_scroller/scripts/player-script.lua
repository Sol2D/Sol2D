require 'math'
require 'player'

local player_id = self.bodyId
local scene = self.scene
local player_mass = scene:getBodyMass(player_id)

local metadata = Player.getMetadata()

local Direction = {
    LEFT = 0,
    RIGHT = 1
}

local Action = {
    IDLE = 0,
    WALK = 1
}

local state = {
    direction = Direction.LEFT,
    action = Action.IDLE
}

function state:set(direction, action)
    if direction ~= self.direction or action ~= self.action then
        local graphic
        if action == Action.IDLE then
            if direction == Direction.LEFT then
                graphic = metadata.graphics.idle_left
            else
                graphic = metadata.graphics.idle_right
            end
        elseif action == Action.WALK then
            if direction == Direction.LEFT then
                graphic = metadata.graphics.walk_left
            else
                graphic = metadata.graphics.walk_right
            end
        end
        self.action = action
        self.direction = direction
        scene:setBodyShapeCurrentGraphic(player_id, metadata.shapes.main, graphic)
    end
end

local function getForce(current_velocity, desired_velocity)
    local change = desired_velocity - current_velocity
    return player_mass * change / (1/60) -- TODO: frame rate
end

local in_air = false
local jump_timeout = 0
local PLAYER_WALK_VELOCITY = 5

sol.heartbeat:subscribe(function()
    local right_key, left_key, space_key = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE
    )

    if jump_timeout > 0 then
        jump_timeout = jump_timeout - 1
    end

    if in_air or space_key then
        local velocity = scene:getBodyLinearVelocity(player_id)
        if math.abs(velocity.y) < 0.01 then
            in_air = false
            if space_key and jump_timeout == 0 then
                jump_timeout = 40
                scene:applyImpulseToBodyCenter(player_id, { x = 0, y = -1300 })
                in_air = true
            end
        end
    end

    if right_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getForce(scene:getBodyLinearVelocity(player_id).x, PLAYER_WALK_VELOCITY),
                y = 0
            }
        )
        state:set(Direction.RIGHT, Action.WALK)
    elseif left_key then
        scene:applyForceToBodyCenter(
            player_id,
            {
                x = getForce(scene:getBodyLinearVelocity(player_id).x, -PLAYER_WALK_VELOCITY),
                y = 0
            }
        )
        state:set(Direction.LEFT, Action.WALK)
    else
        state:set(state.direction, Action.IDLE)
    end
end)
