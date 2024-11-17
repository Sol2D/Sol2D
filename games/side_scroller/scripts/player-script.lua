local Player = require 'player'
local resources = require 'resources'
local level_common = require 'level-common'
local StateMachine = require 'state-machine'

local scene = script.scene
local player = script.body
local METERS_PER_PIXEL = script.arg.metersPerPixel

---@type BodyContactObserver
local contact_observer = script.arg.contactObserver

local player_id = player:getId()
local sound_effect_armor = resources.getSoundEffect(resources.keys.soundEffects.ARMOR)
local sound_effect_swing = resources.getSoundEffect(resources.keys.soundEffects.SWING)
local start_point = scene:getTileMapObjectByName('start-position')
if not start_point then
    error('Start position not found')
end

local attack_body = scene:createBody(
    player:getPosition(),
    {
        type = sol.BodyType.DYNAMIC,
        physics = {
            fixedRotation = true
        },
        shapes = {
            ['player-attack-sensor'] = {
                type = sol.BodyShapeType.POLYGON,
                rect = {
                    x = -14,
                    y = -100,
                    w = 28,
                    h = 100
                },
                physics = {
                    isSensor = true
                }
            }
        }
    })
local attack_joint = scene:createPrismaticJoint({
    bodyA = player,
    bodyB = attack_body,
    localAxisA = { x = 1, y = 0 },
    isLimitEnbaled = true,
    lowerTranslation = -55,
    upperTranslation = 55,
    isSpringEnbaled = true,
    hertz = 100,
    isMotorEnbaled = false,
    maxMotorForce = 7
})

local Direction = {
    LEFT = -1,
    RIGHT = 1
}

local function createState()
    local WALK_VELOCITY = 5
    local JUMP_DELAY = 700

    local State = {
        IDLE = 0,
        WALK = 1,
        JUMP = 2,
        FALL = 3,
        ATTACK = 4
    }

    local Event = {
        NOP = 0,
        H_FORCE = 1,
        FALL = 2,
        JUMP = 3,
        LAND = 4,
        ATTACK = 5
    }

    local player_mass = player:getMass()
    local player_main_shape = player:getShape(Player.keys.shapes.MAIN)
    if not player_main_shape then
        error("Player's main shape not found")
    end

    local inner_state = {
        direction = Direction.RIGHT,
        footings = {},
        jump_timeout = 0,
        attack_start_iteration = 0,
        is_attack_in_progress = false
    }

    local function getFootingVelocity()
        if #inner_state.footings > 0 then
            return inner_state.footings[1].body:getLinearVelocity()
        end
        return { x = 0, y = 0 }
    end

    local function getHorizontalForce(current_velocity, footing_velocity, desired_velocity)
        local change = desired_velocity - (current_velocity - footing_velocity)
        return player_mass * change / (1 / 60) -- TODO: frame rate
    end

    local function activateAttackSensor()
        attack_joint:enableSpring(false)
        attack_joint:enableMotor(true)
        attack_joint:setMotorSpeed(inner_state.direction * 100)
    end

    local function deactivateAttackSensor()
        attack_joint:enableSpring(true)
        attack_joint:enableMotor(false)
        attack_joint:setMotorSpeed(0)
    end

    local idle = {
        enter = function()
            if inner_state.direction == Direction.RIGHT then
                player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.IDLE_RIGHT)
            else
                player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.IDLE_LEFT)
            end
        end
    }

    local walk = {}

    function walk._go()
        local force = getHorizontalForce(
            player:getLinearVelocity().x,
            getFootingVelocity().x,
            WALK_VELOCITY * inner_state.direction
        )
        player:applyForceToCenter({ x = force, y = 0 })
    end

    function walk.enter(args)
        inner_state.direction = args.direction
        if inner_state.direction == Direction.RIGHT then
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.WALK_RIGHT)
        else
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.WALK_LEFT)
        end
        walk._go()
    end

    function walk.continue(args)
        if inner_state.direction ~= args.direction then
            walk.enter(args)
        else
            walk._go()
        end
    end

    local attack = {}

    function attack.enter()
        if inner_state.direction == Direction.RIGHT then
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.ATTACK_RIGHT)
        else
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.ATTACK_LEFT)
        end
        inner_state.is_attack_in_progress = true
        local pack = player_main_shape:getCurrentGraphicsPack()
        if pack then
            inner_state.attack_start_iteration = pack:getCurrentAnimationIteration()
        end
        activateAttackSensor()
    end

    function attack.leave()
        deactivateAttackSensor()
    end

    function attack.canLeave()
        return not inner_state.is_attack_in_progress
    end

    local fly = {}

    function fly.continue(args)
        local force = getHorizontalForce(
            player:getLinearVelocity().x,
            0,
            WALK_VELOCITY * args.direction
        )
        player:applyForceToCenter({ x = force, y = 0 })
    end

    function fly.leave()
        sound_effect_armor:play()
    end

    local jump = {}

    function jump.enter()
        if inner_state.direction == Direction.RIGHT then
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.JUMP_RIGHT)
        else
            player_main_shape:setCurrentGraphics(Player.keys.shapeGraphics.JUMP_LEFT)
        end
        sound_effect_swing:play()
        player:applyImpulseToCenter({ x = 0, y = -1300 })
        inner_state.jump_timeout = JUMP_DELAY
    end

    function jump.canEnter()
        return inner_state.jump_timeout == 0
    end

    local state_machine = StateMachine.new(
        State.IDLE,
        {
            { source = State.IDLE,   target = State.WALK,   event = Event.H_FORCE, onEnter = walk.enter,    onLeave = nil,          canEnter = nil,           canLeave = nil },
            { source = State.IDLE,   target = State.JUMP,   event = Event.JUMP,    onEnter = jump.enter,    onLeave = nil,          canEnter = jump.canEnter, canLeave = nil },
            { source = State.IDLE,   target = State.ATTACK, event = Event.ATTACK,  onEnter = attack.enter,  onLeave = nil,          canEnter = nil,           canLeave = nil },
            { source = State.IDLE,   target = State.FALL,   event = Event.FALL,    onEnter = nil,           onLeave = nil,          canEnter = nil,           canLeave = nil },

            { source = State.WALK,   target = State.WALK,   event = Event.H_FORCE, onEnter = walk.continue, onLeave = nil,          canEnter = nil,           canLeave = nil },
            { source = State.WALK,   target = State.JUMP,   event = Event.JUMP,    onEnter = jump.enter,    onLeave = nil,          canEnter = jump.canEnter, canLeave = nil },
            { source = State.WALK,   target = State.IDLE,   event = Event.NOP,     onEnter = idle.enter,    onLeave = nil,          canEnter = nil,           canLeave = nil },
            { source = State.WALK,   target = State.FALL,   event = Event.FALL,    onEnter = nil,           onLeave = nil,          canEnter = nil,           canLeave = nil },

            { source = State.JUMP,   target = State.IDLE,   event = Event.LAND,    onEnter = idle.enter,    onLeave = fly.leave,    canEnter = nil,           canLeave = nil },
            { source = State.JUMP,   target = State.JUMP,   event = Event.H_FORCE, onEnter = fly.continue,  onLeave = nil,          canEnter = nil,           canLeave = nil },

            { source = State.FALL,   target = State.IDLE,   event = Event.LAND,    onEnter = idle.enter,    onLeave = fly.leave,    canEnter = nil,           canLeave = nil },
            { source = State.FALL,   target = State.FALL,   event = Event.H_FORCE, onEnter = fly.continue,  onLeave = nil,          canEnter = nil,           canLeave = nil },

            { source = State.ATTACK, target = State.IDLE,   event = Event.NOP,     onEnter = idle.enter,    onLeave = attack.leave, canEnter = nil,           canLeave = attack.canLeave },
            { source = State.ATTACK, target = State.ATTACK, event = Event.ATTACK,  onEnter = attack.enter,  onLeave = attack.leave, canEnter = nil,           canLeave = attack.canLeave },
            { source = State.ATTACK, target = State.WALK,   event = Event.H_FORCE, onEnter = walk.enter,    onLeave = attack.leave, canEnter = nil,           canLeave = attack.canLeave },
            { source = State.ATTACK, target = State.FALL,   event = Event.FALL,    onEnter = nil,           onLeave = attack.leave, canEnter = nil,           canLeave = nil }
        }
    )

    local prepared_events = {}

    return {
        addFooting = function(footing)
            if #inner_state.footings == 0 then
                table.insert(
                    prepared_events, 1, { event = Event.LAND }
                )
            else
                for _, f in ipairs(inner_state.footings) do
                    if f.bodyId == footing.bodyId and f.shapeKey == footing.shapeKey then
                        return
                    end
                end
            end
            table.insert(inner_state.footings, footing)
        end,

        removeFooting = function(body_id, shape_key)
            for index, value in ipairs(inner_state.footings) do
                if value.bodyId == body_id and value.shapeKey == shape_key then
                    table.remove(inner_state.footings, index)
                    break
                end
            end
            if #inner_state.footings == 0 then
                table.insert(
                    prepared_events, 1, { event = Event.FALL }
                )
            end
        end,

        applyHorizontalForce = function(direction)
            table.insert(
                prepared_events,
                {
                    event = Event.H_FORCE,
                    args = {
                        direction = direction
                    }
                }
            )
        end,

        beginJump = function()
            table.insert(
                prepared_events,
                { event = Event.JUMP }
            )
        end,

        beginAttack = function()
            table.insert(
                prepared_events,
                { event = Event.ATTACK }
            )
        end,

        completeStep = function(time_passed)
            local events = prepared_events
            prepared_events = {}

            if inner_state.jump_timeout > 0 then
                inner_state.jump_timeout = inner_state.jump_timeout - time_passed
                if inner_state.jump_timeout < 0 then
                    inner_state.jump_timeout = 0
                end
            end
            if inner_state.is_attack_in_progress then
                local pack = player_main_shape:getCurrentGraphicsPack()
                if pack then
                    if inner_state.attack_start_iteration < pack:getCurrentAnimationIteration() then
                        inner_state.is_attack_in_progress = false
                    end
                end
            end

            local processed = false
            for _, e in ipairs(events) do
                processed = state_machine.processEvent(e.event, e.args) or processed
            end
            if not processed then
                state_machine.processEvent(Event.NOP)
            end
        end
    }
end

local state = createState()

scene:subscribeToStep(function(time_passed)
    local right_key, left_key, space_key, left_ctrl = sol.keyboard:getState(
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.SPACE,
        sol.Scancode.L_CTRL
    )
    if space_key then
        state.beginJump()
    end
    if left_ctrl then
        state.beginAttack()
    end
    if right_key then
        state.applyHorizontalForce(Direction.RIGHT)
    elseif left_key then
        state.applyHorizontalForce(Direction.LEFT)
    end
    state.completeStep(time_passed)
end)

contact_observer.setSensorBeginContactListener(player_id, function(sensor, visitor)
    if sensor.bodyId == player_id and sensor.shapeKey == Player.keys.shapes.BOTTOM_SENSOR then
        local visitor_body = scene:getBody(visitor.bodyId)
        if visitor_body then
            state.addFooting({
                bodyId = visitor.bodyId,
                body = visitor_body,
                shapeKey = visitor.shapeKey
            })
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
        state.removeFooting(visitor.bodyId, visitor.shapeKey)
    end
end)
