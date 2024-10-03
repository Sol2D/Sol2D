local keys = require 'resources.keys'
local BodyContactObserver = require 'body-contact-observer'

local METERS_PER_PIXEL = 0.01

---@param point Point
---@return Point
local function pixelPointToPhisical(point)
    return {
        x = point.x * METERS_PER_PIXEL,
        y = point.y * METERS_PER_PIXEL
    }
end

---@param global_store sol.Store
---@param scene sol.Scene
---@param args any
local function createPlayer(global_store, scene, args)
    local proto = global_store:getBodyPrototype(keys.bodies.player)
    if proto == nil then
        error('Unable to load player prototype')
    end
    local start_postion = { x = 410, y = 200 }
    local start_point = scene:getTileMapObjectByName('start-position')
    if start_point then
        start_postion = start_point.position
    end
    local player = scene:createBody(
        pixelPointToPhisical(start_postion),
        proto,
        args
    )
    local main_shape = player:getShape(keys.shapes.player.main)
    if main_shape then
        main_shape:setCurrentGraphics(keys.shapeGraphics.player.idleRight)
    else
        error('There is no shape ' .. keys.shapes.player.main .. ' in ' .. keys.bodies.player)
    end
    scene:setFollowedBody(player)
end

---@param global_store sol.Store
---@param scene sol.Scene
local function createFlyingPlatforms(global_store, scene)
    local proto = global_store:getBodyPrototype(keys.bodies.flyingPlatform3)
    if proto == nil then
        error('Unable to load flying platform 3 prototype')
    end
    local platfroms = scene:getTileMapObjectsByClass('platform')
    for _, platfrom in ipairs(platfroms) do
        local points = {}
        for _, point in ipairs(platfrom.points) do
            table.insert(
                points,
                pixelPointToPhisical({
                    x = platfrom.position.x + point.x,
                    y = platfrom.position.y + point.y
                })
            )
        end
        local platform = scene:createBody(points[1], proto, { points = points })
        local platform_shape = platform:getShape(keys.shapes.oneWayPlatfrom.main)
        if platform_shape then
            platform_shape:setCurrentGraphics(keys.shapeGraphics.flyingPlatform3.main)
        else
            error(
                'There is not shape ' .. keys.shapeGraphics.flyingPlatform3.main ..
                ' in ' .. keys.bodies.flyingPlatform3
            )
        end
    end
end

---@param contact PreSolveContact
---@return boolean
local function preSolveContact(contact)
    local player_side = nil
    local platform_side = nil
    local sign = 0
    if contact.sideA.shapeKey == keys.shapes.player.main then
        player_side = contact.sideA
        platform_side = contact.sideB
        sign = 1
    elseif contact.sideB.shapeKey == keys.shapes.player.main then
        player_side = contact.sideB
        platform_side = contact.sideA
        sign = -1
    end
    if player_side == nil or platform_side == nil or platform_side.shapeKey ~= keys.shapes.oneWayPlatfrom.main then
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
local function createContactSubscriptions(scene, body_contact_observer)
    local begin_contact_subscription = scene:subscribeToBeginContact(function (contact)
        body_contact_observer.callBeginContact(contact)
    end)
    local end_contact_subscription = scene:subscribeToEndContact(function(contact)
        body_contact_observer.callEndContact(contact)
    end)
    local begin_sensor_constact_subscription = scene:subscribeToSensorBeginContact(function(contact)
        body_contact_observer.callSensorBeginContact(contact)
    end)
    local end_sensor_constact_subscription = scene:subscribeToSensorEndContact(function(contact)
        body_contact_observer.callSensorEndContact(contact)
    end)
    return {
        destroy = function ()
            scene:unsubscribeFromBeginContact(begin_contact_subscription)
            scene:unsubscribeFromEndContact(end_contact_subscription)
            scene:unsubscribeFromSensorBeginContact(begin_sensor_constact_subscription)
            scene:unsubscribeFromSesnsorEndContact(end_sensor_constact_subscription)
        end
    }
end

---@param global_store sol.Store
---@param local_store sol.Store
local function createScene(global_store, local_store)
    local scene = local_store:createScene(
        'main',
        {
            gravity = { x = 0, y = 80 },
            metersPerPixel = METERS_PER_PIXEL
        }
    )
    scene:loadTileMap('tilemaps/level-01.tmx')
    scene:createBodiesFromMapObjects('obstacle')
    scene:createBodiesFromMapObjects(
        'one-way-platfrom',
        {
            shapeKey = keys.shapes.oneWayPlatfrom.main,
            shapePhysics = {
                isPreSolveEnabled = true
            }
        }
    )
    local observer = BodyContactObserver.new()
    createPlayer(global_store, scene, { contactObserver = observer })
    createFlyingPlatforms(global_store, scene)
    local pre_solve_subscription = scene:subscribeToPreSolveContact(preSolveContact)
    local contact_subsciptions = createContactSubscriptions(scene, observer)
    return {
        scene = scene,
        destroy = function ()
            contact_subsciptions.destroy()
            scene:unsubscribePreSolveContact(pre_solve_subscription)
        end
    }
end

local store_counter = 1

---@param global_store sol.Store
local function run(global_store)
    local local_store_key = 'level-01_' .. store_counter
    local local_store = sol.stores:createStore(local_store_key)
    store_counter = store_counter + 1
    local level_meta = {}
    level_meta.__index = level_meta
    local scene_data = createScene(global_store, local_store)
    local main_view = local_store:createView('main')
    local main_fragment = main_view:createFragment({}) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
    main_view:bindFragment(main_fragment, scene_data.scene)
    sol.window:setView(main_view)
    function level_meta.destroy()
        scene_data.destroy()
        sol.stores:deleteStore(local_store_key)
    end
    return setmetatable({}, level_meta)
end

return { run = run }
