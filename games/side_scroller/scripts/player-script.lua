local Player = require 'player'
local StateMachine = require 'state-machine'

local scene = script.scene
local WALK_VELOCITY = 5

local State = {
    IDLE = 'idel',
    WALK = 'walk',
    JUMP = 'jump',
    FALL = 'fall',
    ATTACK = 'attack'
}

local Event = {
    NOP = 0,
    H_FORCE = 1,
    FALL = 2,
    JUMP = 3,
    LAND = 4,
    ATTACK = 5
}

local Direction = {
    LEFT = -1,
    RIGHT = 1
}

local player = {
    body = script.body,
    mainShape = script.body:getShape(Player.keys.shapes.MAIN),
    mass = script.body:getMass(),
    direction = Direction.RIGHT,
    footings = {
        items = {}
    }
}

if not player.mainShape then
    error("Player's main shape not found")
end

local function createStateMachine()
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

    local current = {
        state = nil,
        direction = nil
    }

    local function updateAnimation()
        if current.state and current.direction ~= player.direction then
            current.direction = player.direction
            player.mainShape:setCurrentGraphics(animations[current.state][current.direction])
        end
    end

    ---@param state StateMachineStateKey
    local function setAnimation(state)
        if state ~= current.state or player.direction ~= current.direction then
            current.direction = player.direction
            current.state = state
            player.mainShape:setCurrentGraphics(animations[current.state][current.direction])
        end
    end

    setAnimation(State.IDLE)

    local states = {
        [State.IDLE] = {
            enter = function(event)
                setAnimation(State.IDLE)
            end,
            update = function(dt)
                updateAnimation()
            end
        },
        [State.WALK] = {
            enter = function(event)
                setAnimation(State.WALK)
            end,
            update = function(dt)
                updateAnimation()
            end
        },
        [State.JUMP] = {
            enter = function(event)
                setAnimation(State.JUMP)
            end,
            update = function(dt)
                -- FIXME: direction cannot be changes!
            end
        },
        [State.FALL] = {
            enter = function(event)
                setAnimation(State.FALL)
            end,
            update = function(dt)
                -- FIXME: direction cannot be changes!
            end
        },
        [State.ATTACK] = {
            enter = function(event)
                setAnimation(State.FALL)
            end,
            update = function(dt)
                -- FIXME: direction cannot be changes!
            end
        }
    }

    states[State.IDLE].enter(Event.NOP)

    return StateMachine.new(
        states,
        State.IDLE,
        {
            { from = State.IDLE,   event = Event.H_FORCE, to = State.WALK },
            { from = State.IDLE,   event = Event.FALL,    to = State.FALL },
            { from = State.IDLE,   event = Event.ATTACK,  to = State.ATTACK },
            { from = State.IDLE,   event = Event.JUMP,    to = State.JUMP },
            { from = State.FALL,   event = Event.LAND,    to = State.IDLE },
            { from = State.JUMP,   event = Event.LAND,    to = State.IDLE },
            { from = State.WALK,   event = Event.ATTACK,  to = State.ATTACK },
            { from = State.WALK,   event = Event.JUMP,    to = State.JUMP },
            { from = State.WALK,   event = Event.NOP,     to = State.IDLE },
            { from = State.ATTACK, event = Event.NOP,     to = State.IDLE }
        }
    )
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

    if right_key then
        player.direction = Direction.RIGHT
        sm.processEvent(Event.H_FORCE)
    elseif left_key then
        player.direction = Direction.LEFT
        sm.processEvent(Event.H_FORCE)
    else
        sm.processEvent(Event.NOP)
    end
    sm.update(dt)
end)
