local keys = require 'resources.keys'
local ButtonFactory = require 'resources.button-factory'
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
---@param args any
local function createButton(global_store, scene, args)
    local button_position_object = scene:getTileMapObjectByName('button')
    if not button_position_object then
        error('There is no button-position object on the tile map')
    end
    ButtonFactory.createButton(
        global_store,
        scene,
        pixelPointToPhisical(button_position_object.position),
        { layer = keys.tilemap.level02.layers.background }
    )
end

---@param scene sol.Scene
---@param body_contact_observer BodyContactObserver
local function createContactSubscriptions(scene, body_contact_observer)
    local begin_contact_subscription = scene:subscribeToBeginContact(function(contact)
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
        destroy = function()
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
    scene:loadTileMap('tilemaps/level-02.tmx')
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
    scene:createBodiesFromMapObjects(
        'water',
        {
            shapeKey = keys.shapes.water.main,
            shapePhysics = {
                isSensor = true
            }
        }
    )
    local observer = BodyContactObserver.new()
    createPlayer(
        global_store,
        scene,
        {
            METERS_PER_PIXEL = METERS_PER_PIXEL,
            contactObserver = observer
        }
    )
    createButton(global_store, scene)
    local contact_subsciptions = createContactSubscriptions(scene, observer)
    return {
        scene = scene,
        destroy = function()
            contact_subsciptions.destroy()
        end
    }
end

---@param global_store sol.Store
local function run(global_store)
    local local_store = sol.stores:createStore(keys.stores.level02)

    local music = local_store:createMusic('level-01', 'sounds/level-01/level-01.wav')
    music:loop(-1)

    local level_meta = {}
    level_meta.__index = level_meta
    local scene_data = createScene(global_store, local_store)
    local main_view = local_store:createView('main')
    local main_fragment = main_view:createFragment({}) -- TODO: allow creation without arguments, default: 0, 0, 100%, 100%
    main_view:bindFragment(main_fragment, scene_data.scene)
    sol.window:setView(main_view)
    function level_meta.destroy()
        scene_data.destroy()
        sol.stores:deleteStore(keys.stores.level01)
    end

    return setmetatable({}, level_meta)
end

return { run = run }