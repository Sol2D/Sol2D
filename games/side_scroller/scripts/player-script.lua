local Player = require 'player'
local StateMachine = require 'state-machine'
local resources = require 'resources'

local scene = script.scene
local player_body_id = script.body:getId()
local sound_effect_armor = resources.getSoundEffect(resources.keys.soundEffects.ARMOR)
local sound_effect_swing = resources.getSoundEffect(resources.keys.soundEffects.SWING)

local State = {
    IDLE = 'idel',
    WALK = 'walk',
    JUMP = 'jump',
    FALL = 'fall',
    ATTACK = 'attack'
}

local Event = {
    NOP = 0,
    MOVE = 1,
    FALL = 2,
    JUMP = 3,
    ATTACK = 4
}

local Direction = {
    LEFT = -1,
    RIGHT = 1
}

---@class UpdateData
---@field dt integer
---@field isMoving boolean

local function createStateMachine()
    local WALK_VELOCITY = 5
    local JUMP_DELAY = 70
    local JUMP_IMPULSE = { x = 0, y = -1100 } -- FIXME: depends on FPS (?)

    local player = {
        body = script.body,
        mainShape = script.body:getShape(Player.keys.shapes.MAIN),
        mass = script.body:getMass(),
        direction = Direction.RIGHT,
        animation = {
            state = nil,
            direction = nil
        },
        footings = {},
        isJumping = false,
        jumpTimeout = 0
    }

    if not player.mainShape then
        error("Player's main shape not found")
    end

    local animations = {
        [State.IDLE] = {
            [Direction.LEFT] = Player.keys.shapeGraphics.IDLE_LEFT,
            [Direction.RIGHT] = Player.keys.shapeGraphics.IDLE_RIGHT
        },
        [State.WALK] = {
            [Direction.LEFT] = Player.keys.shapeGraphics.WALK_LEFT,
            [Direction.RIGHT] = Player.keys.shapeGraphics.WALK_RIGHT
        },
        [State.JUMP] = {
            [Direction.LEFT] = Player.keys.shapeGraphics.JUMP_LEFT,
            [Direction.RIGHT] = Player.keys.shapeGraphics.JUMP_RIGHT
        },
        [State.FALL] = {
            [Direction.LEFT] = Player.keys.shapeGraphics.IDLE_LEFT,
            [Direction.RIGHT] = Player.keys.shapeGraphics.IDLE_RIGHT
        },
        [State.ATTACK] = {
            [Direction.LEFT] = Player.keys.shapeGraphics.ATTACK_LEFT,
            [Direction.RIGHT] = Player.keys.shapeGraphics.ATTACK_RIGHT
        }
    }

    function player.syncAnimationDirection()
        if player.animation.direction ~= player.direction then
            player.animation.direction = player.direction
            player.mainShape:setCurrentGraphics(animations[player.animation.state][player.animation.direction])
        end
    end

    ---@param state StateMachineStateKey
    function player.setAnimation(state)
        if state ~= player.animation.state or player.direction ~= player.animation.direction then
            player.animation.state = state
            player.animation.direction = player.direction
            player.mainShape:setCurrentGraphics(animations[player.animation.state][player.animation.direction])
        end
    end

    function player.isInAir()
        return #player.footings == 0
    end

    function player.getFootingVelocity()
        if #player.footings > 0 then
            return player.footings[1].body:getLinearVelocity()
        end
        return { x = 0, y = 0 }
    end

    function player.getHorizontalForce(current_velocity, footing_velocity, desired_velocity)
        local change = desired_velocity - (current_velocity - footing_velocity)
        return player.mass * change / (1 / 60) -- FIXME: depends on FPS
    end

    function player.move()
        local desired_velocity = WALK_VELOCITY
        if player.direction == Direction.LEFT then
            desired_velocity = -desired_velocity
        end
        local footing_velocity = 0
        if not player.isInAir() then
            footing_velocity = player.getFootingVelocity().x
        end
        local force = player.getHorizontalForce(player.body:getLinearVelocity().x, footing_velocity, desired_velocity)
        player.body:applyForceToCenter({ x = force, y = 0 })
    end

    player.setAnimation(State.IDLE)

    local states = {
        [State.IDLE] = {
            enter = function(event)
                player.setAnimation(State.IDLE)
            end,
            update = function(data)
                player.syncAnimationDirection()
            end
        },
        [State.WALK] = {
            enter = function(event)
                player.setAnimation(State.WALK)
            end,
            update = function(data)
                player.syncAnimationDirection();
                player.move()
            end
        },
        [State.JUMP] = {
            canEnter = function()
                return not player.isJumping and not player.isInAir() and player.jumpTimeout == 0
            end,
            enter = function(event)
                player.setAnimation(State.JUMP)
                player.isJumping = true
                player.body:applyImpulseToCenter(JUMP_IMPULSE)
                player.jumpTimeout = JUMP_DELAY
                sound_effect_swing:play()
            end,
            update = function(data)
                if not player.isJumping and player.jumpTimeout == 0 then
                    player.body:applyImpulseToCenter(JUMP_IMPULSE)
                    player.jumpTimeout = JUMP_DELAY
                end
                if data.isMoving then
                    player.move()
                end
            end,
            canLeave = function()
                return not player.isJumping
            end,
            leave = function()
                -- The direction changed in the air does not change the facing direction.
                -- Therefore, we restore the direction to prevent an unexpected change.
                player.direction = player.animation.direction
                sound_effect_armor:play()
            end
        },
        [State.FALL] = {
            enter = function(event)
                player.setAnimation(State.FALL)
            end,
            update = function(data)
                if data.isMoving then
                    player.move()
                end
            end,
            canLeave = function()
                return not player.isInAir()
            end,
            leave = function()
                -- The direction changed in the air does not change the facing direction.
                -- Therefore, we restore the direction to prevent an unexpected change.
                player.direction = player.animation.direction
                sound_effect_armor:play()
            end
        },
        [State.ATTACK] = {
            enter = function(event)
                player.setAnimation(State.ATTACK)
                -- TODO: enable sensor
            end,
            update = function(data)
            end,
            onLeave = function()
                -- TODO: disable sensor
            end
        }
    }

    states[State.IDLE].enter(Event.NOP)

    local sm = StateMachine.new(
        states,
        State.IDLE,
        {
            { from = State.IDLE,   event = Event.MOVE,   to = State.WALK },
            { from = State.IDLE,   event = Event.FALL,   to = State.FALL },
            { from = State.IDLE,   event = Event.ATTACK, to = State.ATTACK },
            { from = State.IDLE,   event = Event.JUMP,   to = State.JUMP },
            { from = State.FALL,   event = Event.NOP,    to = State.IDLE },
            { from = State.FALL,   event = Event.MOVE,   to = State.WALK },
            { from = State.JUMP,   event = Event.NOP,    to = State.IDLE },
            { from = State.JUMP,   event = Event.MOVE,   to = State.WALK },
            { from = State.WALK,   event = Event.ATTACK, to = State.ATTACK },
            { from = State.WALK,   event = Event.JUMP,   to = State.JUMP },
            { from = State.WALK,   event = Event.NOP,    to = State.IDLE },
            { from = State.WALK,   event = Event.FALL,   to = State.FALL },
            { from = State.ATTACK, event = Event.MOVE,   to = State.WALK },
            { from = State.ATTACK, event = Event.NOP,    to = State.IDLE },
            { from = State.ATTACK, event = Event.FALL,   to = State.FALL },
        }
    )

    local facade = {}
    facade.__index = facade

    ---@param direction integer
    function facade.setDirection(direction)
        player.direction = direction
    end

    ---@param body_id integer
    ---@param shape_key string
    function facade.addFooting(body_id, shape_key)
        local body = scene:getBody(body_id)
        if not body then
            error('Body ' .. tostring(body_id) .. ' not found')
        end
        local was_in_air = player.isInAir()
        table.insert(player.footings, { body = body, bodyId = body_id, shapeKey = shape_key })
        if was_in_air then
            player.isJumping = false
            sm.processEvent(Event.NOP)
        end
    end

    ---@param body_id integer
    ---@param shape_key string
    function facade.removeFooting(body_id, shape_key)
        for idx, footing in ipairs(player.footings) do
            if footing.bodyId == body_id and footing.shapeKey == shape_key then
                table.remove(player.footings, idx)
                break;
            end
        end
        if #player.footings == 0 and not player.isJumping then
            sm.processEvent(Event.FALL)
        end
    end

    ---@param data UpdateData
    function facade.update(data)
        sm.update(data)
        if not player.isJumping and player.jumpTimeout > 0 then
            player.jumpTimeout = player.jumpTimeout - data.dt
            if player.jumpTimeout < 0 then
                player.jumpTimeout = 0
            end
        end
    end

    return setmetatable(facade, sm)
end

local sm = createStateMachine()

scene:subscribeToStep(function(dt)
    local
    right_key,
    left_key,
    space_key,
    left_ctrl =
        sol.keyboard:getState(
            sol.Scancode.RIGHT_ARROW,
            sol.Scancode.LEFT_ARROW,
            sol.Scancode.SPACE,
            sol.Scancode.L_CTRL
        )

    local direction = nil
    if right_key then
        direction = Direction.RIGHT
        sm.setDirection(Direction.RIGHT)
    elseif left_key then
        direction = Direction.LEFT
        sm.setDirection(Direction.LEFT)
    end

    if left_ctrl then
        sm.processEvent(Event.ATTACK)
    elseif space_key then
        sm.processEvent(Event.JUMP)
    elseif direction then
        sm.processEvent(Event.MOVE)
    else
        sm.processEvent(Event.NOP)
    end

    sm.update({
        dt = dt,
        isMoving = direction ~= nil
    })
end)

scene:subscribeToSensorBeginContact(function(contact)
    if
        contact.sensor.bodyId == player_body_id and
        contact.sensor.shapeKey == Player.keys.shapes.BOTTOM_SENSOR and
        contact.visitor.bodyId ~= player_body_id
    then
        sm.addFooting(contact.visitor.bodyId, contact.visitor.shapeKey)
    end
end)

scene:subscribeToSensorEndContact(function(contact)
    if
        contact.sensor.bodyId == player_body_id and
        contact.sensor.shapeKey == Player.keys.shapes.BOTTOM_SENSOR and
        contact.visitor.bodyId ~= player_body_id
    then
        sm.removeFooting(contact.visitor.bodyId, contact.visitor.shapeKey)
    end
end)
