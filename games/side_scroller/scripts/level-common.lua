local Player = require 'player'
local BodyContactObserver = require 'body-contact-observer'
local Platform = require 'platform'

local keys = {
    shapes = {
        oneWayPlatfrom = 'one-way-platform',
        water = 'water'
    }
}

local METERS_PER_PIXEL = 0.01
local layers = {
    BACKGROUND = 'background',
    GROUND_1 = 'ground-1',
    GROUND_2 = 'ground-2'
}

---@param point Point
---@return Point
local function pixelPontToMeters(point)
    return {
        x = point.x * METERS_PER_PIXEL,
        y = point.y * METERS_PER_PIXEL
    }
end

local level = {}
level.__index = level
level.pixelPontToMeters = pixelPontToMeters

---@param scene sol.Scene
---@return Point
local function getPlayerStartPosition(scene)
    local start_point = scene:getTileMapObjectByName('start-position')
    if not start_point then
        error('There is no object named start-position on the fragment map')
    end
    return pixelPontToMeters(start_point.position)
end

---@param scene sol.Scene
local function createFlyingPlatforms(scene)
    local platfroms = scene:getTileMapObjectsByClass('platform')
    for _, platfrom in ipairs(platfroms) do
        local points = {}
        for _, point in ipairs(platfrom.points) do
            table.insert(
                points,
                pixelPontToMeters({
                    x = platfrom.position.x + point.x,
                    y = platfrom.position.y + point.y
                })
            )
        end
        Platform.new(
            scene,
            points[1],
            {
                bodyType = sol.BodyType.KINEMATIC,
                length = 3,
                layer = layers.GROUND_2,
                script = 'flying-platform.lua',
                scriptArgument = { points = points }
            }
        )
    end
end

---@param contact PreSolveContact
---@return boolean
local function preSolveContact(contact)
    local player_side = nil
    local platform_side = nil
    local sign = 0
    if contact.sideA.shapeKey == Player.keys.shapes.main then
        player_side = contact.sideA
        platform_side = contact.sideB
        sign = 1
    elseif contact.sideB.shapeKey == Player.keys.shapes.main then
        player_side = contact.sideB
        platform_side = contact.sideA
        sign = -1
    end
    if player_side == nil or platform_side == nil or platform_side.shapeKey ~= keys.shapes.oneWayPlatfrom then
        return true
    end

    if contact.manifold.normal.y * sign < 0.95 then
        return false;
    end

    for _, point in ipairs(contact.manifold.points) do
        if point.separation > -0.2 then
            return true
        end
    end

    return false
end

---@param scene sol.Scene
---@param body_contact_observer BodyContactObserver
---@param callbacks DerivedInitResult
---@param on_finish function?
local function createContactSubscriptions(scene, body_contact_observer, callbacks, on_finish)
    local begin_contact_subscription = scene:subscribeToBeginContact(function(contact)
        body_contact_observer.callBeginContact(contact)
    end)
    local end_contact_subscription = scene:subscribeToEndContact(function(contact)
        body_contact_observer.callEndContact(contact)
    end)
    local begin_sensor_constact_subscription = scene:subscribeToSensorBeginContact(function(contact)
        if contact.sensor.tileMapObjectId == 13 and on_finish then -- FIXME: + tile map object NAME and CLASS
            on_finish()
        end
        if callbacks.onSensorBeginContact then
            callbacks.onSensorBeginContact(contact)
        end
        body_contact_observer.callSensorBeginContact(contact)
    end)
    local end_sensor_constact_subscription = scene:subscribeToSensorEndContact(function(contact)
        if callbacks.onSensorEndContact then
            callbacks.onSensorEndContact(contact)
        end
        body_contact_observer.callSensorEndContact(contact)
    end)
    local pre_solve_subscription = scene:subscribeToPreSolveContact(preSolveContact)
    return {
        destroy = function()
            scene:unsubscribeFromBeginContact(begin_contact_subscription)
            scene:unsubscribeFromEndContact(end_contact_subscription)
            scene:unsubscribeFromSensorBeginContact(begin_sensor_constact_subscription)
            scene:unsubscribeFromSesnsorEndContact(end_sensor_constact_subscription)
            scene:unsubscribeFromPreSolveContact(pre_solve_subscription)
        end
    }
end

---@param on_finish function
function level:run(on_finish)
    local store = sol.stores:createStore(self.name)
    local scene = store:createScene(
        self.name,
        {
            gravity = { x = 0, y = 80 },
            metersPerPixel = METERS_PER_PIXEL
        }
    )
    if not scene:loadTileMap(self.tileMap) then
        error('An error occurred while loading the tile map from ' .. self.tileMap)
    end
    local derived
    if self.init then
        derived = self:init(scene)
    else
        derived = {}
    end
    local body_contact_observer = BodyContactObserver.new()
    local contact_subsciptions = createContactSubscriptions(scene, body_contact_observer, derived, on_finish)
    local player = Player.new(
        scene,
        getPlayerStartPosition(scene),
        {
            contactObserver = body_contact_observer,
            metersPerPixel = METERS_PER_PIXEL
        }
    )
    scene:setFollowedBody(player)
    scene:createBodiesFromMapObjects('obstacle')
    scene:createBodiesFromMapObjects('sensor', { shapePhysics = { isSensor = true } })
    scene:createBodiesFromMapObjects(
        'one-way-platfrom',
        {
            shapeKey = keys.shapes.oneWayPlatfrom,
            shapePhysics = {
                isPreSolveEnabled = true
            }
        }
    )
    scene:createBodiesFromMapObjects(
        'water',
        {
            shapeKey = keys.shapes.water,
            shapePhysics = {
                isSensor = true
            }
        }
    )
    createFlyingPlatforms(scene)

    local main_view = store:createView(self.name)
    local main_fragment = main_view:createFragment({}) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
    main_view:bindFragment(main_fragment, scene)
    sol.window:setView(main_view)

    local music = store:createMusic(self.name, self.music)
    if music then
        music:loop(-1)
    else
        error('An error occurred while loading the music from ' .. self.music)
    end

    local result = {}
    result.__index = result
    function result.destroy()
        contact_subsciptions.destroy()
        sol.stores:deleteStore(self.name)
    end
    return setmetatable({}, result)
end

---@class DerivedLevel
---@field name string
---@field tileMap string
---@field music string
---@field init fun(self: DerivedLevel, scene: sol.Scene)?
---@return DerivedInitResult

---@class DerivedInitResult
---@field onSensorBeginContact SensorContactCallback?
---@field onSensorEndContact SensorContactCallback?

local module = {
    keys = keys,
    layers = layers
}
module.__index = module

---@param derived_level DerivedLevel
---@param on_finish function
function module.run(derived_level, on_finish)
    local l = setmetatable(derived_level, level)
    return l:run(on_finish)
end

return setmetatable({}, module)
