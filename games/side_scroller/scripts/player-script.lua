local Player = require 'player'
local resources = require 'resources'
local level_common = require 'level-common'

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
    local FlyState = {
        NONE = 0,
        JUMP = 1,
        FALL = 2,
        LAND = 3
    }
    local Action = {
        NONE = 0,
        WALK = 1,
        ATACK = 2
    }

    local player_mass = player:getMass()
    local player_main_shape = player:getShape(Player.keys.shapes.MAIN)
    if not player_main_shape then
        error("Player's main shape not found")
    end

    local inner_state = {
        fly_state = FlyState.NONE,
        direction = Direction.RIGHT,
        action = Action.NONE,
        footings = {},
        current_graphics = Player.keys.shapeGraphics.IDLE_RIGHT,
        jump_timeout = 0,
        attack_start_iteration = 0
    }

    local state = {}

    local function activateAtack()
        attack_joint:enableSpring(false)
        attack_joint:enableMotor(true)
        attack_joint:setMotorSpeed(inner_state.direction * 100)
    end

    local function deactivateAtack()
        attack_joint:enableSpring(true)
        attack_joint:enableMotor(false)
        attack_joint:setMotorSpeed(0)
    end

    function state.addFooting(footing)
        if #inner_state.footings == 0 then
            sound_effect_armor:play()
        end
        table.insert(inner_state.footings, footing)
        inner_state.fly_state = FlyState.LAND
    end

    function state.removeFooting(body_id, shape_key)
        for index, value in ipairs(inner_state.footings) do
            if value.bodyId == body_id and value.shapeKey == shape_key then
                table.remove(inner_state.footings, index)
                break
            end
        end
        if #inner_state.footings == 0 and inner_state.fly_state == FlyState.NONE then
            inner_state.fly_state = FlyState.FALL
        end
    end

    local function canJump()
        return
            inner_state.fly_state == FlyState.NONE and
            inner_state.jump_timeout == 0 and
            inner_state.action ~= Action.ATACK
    end

    function state.startJump()
        if not canJump() then
            return
        end
        inner_state.fly_state = FlyState.JUMP
        sound_effect_swing:play()
        player:applyImpulseToCenter({ x = 0, y = -1300 })
        inner_state.jump_timeout = JUMP_DELAY
    end

    local function catAtack()
        return inner_state.fly_state == FlyState.NONE
    end

    function state.startAtack()
        if catAtack() then
            inner_state.action = Action.ATACK
            activateAtack()
        end
    end

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

    local function canChangeDirection()
        return inner_state.action ~= Action.ATACK
    end

    function state.applyHorizontalForce(direction)
        if not canChangeDirection() then
            return
        end
        inner_state.direction = direction
        if inner_state.fly_state == FlyState.NONE then
            inner_state.action = Action.WALK
            player:applyForceToCenter({
                x = getHorizontalForce(
                    player:getLinearVelocity().x,
                    getFootingVelocity().x,
                    WALK_VELOCITY * direction
                ),
                y = 0
            })
        else
            player:applyForceToCenter({
                x = getHorizontalForce(
                    player:getLinearVelocity().x,
                    0,
                    WALK_VELOCITY * direction
                ),
                y = 0
            })
        end
    end

    local function resetAction()
        if inner_state.action == Action.ATACK then
            local graphics = player_main_shape:getCurrentGraphicsPack()
            if not graphics or graphics:getCurrentAnimationIteration() > inner_state.attack_start_iteration then
                inner_state.action = Action.NONE
                deactivateAtack()
            end
        else
            inner_state.action = Action.NONE
        end
    end

    function state.applyStep(time_passed)
        if inner_state.jump_timeout then
            inner_state.jump_timeout = inner_state.jump_timeout - time_passed
            if inner_state.jump_timeout < 0 then
                inner_state.jump_timeout = 0
            end
        end
        local graphics = nil
        if inner_state.fly_state ~= FlyState.NONE then
            if inner_state.fly_state == FlyState.LAND then
                inner_state.fly_state = FlyState.NONE
            elseif inner_state.fly_state == FlyState.JUMP then
                if inner_state.direction == Direction.RIGHT then
                    graphics = Player.keys.shapeGraphics.JUMP_RIGHT
                else
                    graphics = Player.keys.shapeGraphics.JUMP_LEFT
                end
            end
        elseif inner_state.action == Action.WALK then
            if inner_state.direction == Direction.RIGHT then
                graphics = Player.keys.shapeGraphics.WALK_RIGHT
            else
                graphics = Player.keys.shapeGraphics.WALK_LEFT
            end
        elseif inner_state.action == Action.ATACK then
            if inner_state.direction == Direction.RIGHT then
                graphics = Player.keys.shapeGraphics.ATTACK_RIGHT
            else
                graphics = Player.keys.shapeGraphics.ATTACK_LEFT
            end
        else
            if inner_state.direction == Direction.RIGHT then
                graphics = Player.keys.shapeGraphics.IDLE_RIGHT
            else
                graphics = Player.keys.shapeGraphics.IDLE_LEFT
            end
        end
        if graphics and graphics ~= inner_state.current_graphics then
            player_main_shape:setCurrentGraphics(graphics)
            inner_state.current_graphics = graphics
            if inner_state.action == Action.ATACK then
                local gr = player_main_shape:getCurrentGraphicsPack()
                if gr then
                    inner_state.attack_start_iteration = gr:getCurrentAnimationIteration()
                end
            end
        end
        resetAction()
    end

    return state;
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
        state.startJump()
    end
    if left_ctrl then
        state.startAtack()
    end
    if right_key then
        state.applyHorizontalForce(Direction.RIGHT)
    elseif left_key then
        state.applyHorizontalForce(Direction.LEFT)
    end
    state.applyStep(time_passed)
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
