local resources = require 'resources'
local Player = require 'player'
local level_common = require 'level-common'

local player = script.body
local scene = script.scene

local sound_effect_armor = resources.getSoundEffect(resources.keys.soundEffects.ARMOR)
local sound_effect_swing = resources.getSoundEffect(resources.keys.soundEffects.SWING)

local start_point = scene:getTileMapObjectByName('start-position')
if not start_point then
    error('Start position not found')
end

---@type BodyContactObserver
local contact_observer = script.arg.contactObserver

local player_id = player:getId()
local player_mass = player:getMass()
local player_main_shape = player:getShape(Player.keys.shapes.MAIN)
if not player_main_shape then
    error("Player's main shape not found")
end

local METERS_PER_PIXEL = script.arg.metersPerPixel
local WALK_VELOCITY = 5
local JUMP_DELAY = 200

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
    jumping = false,
    footings = {},
    jumpTimeout = 0,
    direction = Direction.RIGHT,
    action = Action.IDLE
}

function state:setAction(direction, action)
    if direction ~= self.direction or action ~= self.action then
        local graphic
        if action == Action.IDLE then
            if direction == Direction.LEFT then
                graphic = Player.keys.shapeGraphics.IDLE_LEFT
            else
                graphic = Player.keys.shapeGraphics.IDLE_RIGHT
            end
        elseif action == Action.WALK then
            if direction == Direction.LEFT then
                graphic = Player.keys.shapeGraphics.WALK_LEFT
            else
                graphic = Player.keys.shapeGraphics.WALK_RIGHT
            end
        elseif action == Action.JUMP then
            if direction == Direction.LEFT then
                graphic = Player.keys.shapeGraphics.JUMP_LEFT
            else
                graphic = Player.keys.shapeGraphics.JUMP_RIGHT
            end
        end
        self.action = action
        self.direction = direction
        player_main_shape:setCurrentGraphics(graphic)
    end
end

local function getFootingVelocity()
    if #state.footings > 0 then
        return state.footings[1].body:getLinearVelocity()
    end
    return { x = 0, y = 0 }
end

local function getHorizontalForce(current_velocity, footing_velocity, desired_velocity)
    local change = desired_velocity - (current_velocity - footing_velocity)
    return player_mass * change / (1 / 60) -- TODO: frame rate
end

scene:subscribeToStep(function(time_passed)
    local right_key, left_key, space_key = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE
    )

    local action
    if state.jumping then
        action = Action.JUMP
    else
        action = Action.IDLE
    end

    local direction = state.direction

    if not state.inAir then
        if state.jumpTimeout > 0 then
            state.jumpTimeout = state.jumpTimeout - time_passed
        elseif space_key then
            state.jumping = true
            state.jumpTimeout = JUMP_DELAY
            player:applyImpulseToCenter({ x = 0, y = -1300 })
            action = Action.JUMP
            sound_effect_swing:play()
        end
    end

    if right_key then
        player:applyForceToCenter(
            {
                x = getHorizontalForce(
                    player:getLinearVelocity().x,
                    getFootingVelocity().x,
                    WALK_VELOCITY
                ),
                y = 0
            }
        )
        direction = Direction.RIGHT
        if not state.jumping then
            action = Action.WALK
        end
    elseif left_key then
        player:applyForceToCenter(
            {
                x = getHorizontalForce(
                    player:getLinearVelocity().x,
                    getFootingVelocity().x,
                    -WALK_VELOCITY
                ),
                y = 0
            }
        )
        direction = Direction.LEFT
        if not state.jumping then
            action = Action.WALK
        end
    end

    state:setAction(direction, action)
end)

contact_observer.setSensorBeginContactListener(player_id, function(sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == Player.keys.shapes.BOTTOM_SENSOR then
        local visitor_body = scene:getBody(visitor.bodyId)
        if visitor_body then
            table.insert(
                state.footings, {
                    bodyId = visitor.bodyId,
                    body = visitor_body,
                    shapeKey = visitor.shapeKey
                })
            if state.inAir then
                sound_effect_armor:play()
                state.inAir = false
            end
            state.jumping = false
        end
    elseif sensor.shapeKey == level_common.keys.shapes.WATER then
        player:setPosition({
            x = start_point.position.x * METERS_PER_PIXEL,
            y = start_point.position.y * METERS_PER_PIXEL
        })
    end
end)

contact_observer.setSensorEndContactListener(player_id, function(sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == Player.keys.shapes.BOTTOM_SENSOR then
        for index, footing in ipairs(state.footings) do
            if footing.bodyId == visitor.bodyId and footing.shapeKey == visitor.shapeKey then
                table.remove(state.footings, index)
                break
            end
        end
        state.inAir = #state.footings == 0
    end
end)
