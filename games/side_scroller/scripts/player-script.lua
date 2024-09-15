require 'player'

local player_id = self.bodyId
local scene = self.scene

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

sol.heartbeat:subscribe(function()
    local right_key, left_key, space_key = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE
    )
    local y_force = 0
    if space_key then
        y_force = -15000
    end
    if right_key then
        scene:applyForce(player_id, { x = 5500, y = y_force })
        state:set(Direction.RIGHT, Action.WALK)
    elseif left_key then
        scene:applyForce(player_id, { x = -5500, y = y_force })
        state:set(Direction.LEFT, Action.WALK)
    else
        if y_force then
            scene:applyForce(player_id, { x = 0, y = y_force })
        end
        state:set(state.direction, Action.IDLE)
    end
end)
