local keys = require 'resources.keys'

---@param store sol.Store
local function createButtonPrototype(store)
    local sprite = store:createSprite('button')
    if not sprite:loadFromFile('sprites/button/button.png') then
        error('Unable to load sprites/button/button.png')
    end
    return store:createBodyPrototype(
        keys.bodies.button,
        {
            type = sol.BodyType.DYNAMIC,
            physics = { fixedRotation = true },
            shapes = {
                [keys.shapes.button.main] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        density = 100,
                        isPreSolveEnabled = true
                    },
                    rect = { x = 0, y = 0, w = 119, h = 26 },
                    graphics = {
                        [keys.shapeGraphics.button.main] = {
                            animationIterations = 0,
                            frames = { { sprites = { { sprite = sprite } } } }
                        }
                    }
                }
            }
        }
    )
end

---@param store sol.Store
local function createHullPrototype(store)
    local sprite = store:createSprite('button-hull')
    if not sprite:loadFromFile('sprites/button/hull.png') then
        error('Unable to load sprites/button/hull.png')
    end
    return store:createBodyPrototype(
        keys.bodies.buttonHull,
        {
            type = sol.BodyType.STATIC,
            shapes = {
                [keys.shapes.button.hull] = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = 0, y = 0, w = 129, h = 40 },
                    graphics = {
                        [keys.shapeGraphics.button.hull] = {
                            animationIterations = 0,
                            frames = { { sprites = { { sprite = sprite } } } }
                        }
                    }
                }
            }
        }
    )
end

---@param store sol.Store
---@param scene sol.Scene
---@param position Point
---@param options { layer: string }?
local function createButton(store, scene, position, options)
    local button_prototype = store:getBodyPrototype(keys.bodies.button)
    local hull_prototype = store:getBodyPrototype(keys.bodies.buttonHull)
    if not button_prototype then
        button_prototype = createButtonPrototype(store)
    end
    if not hull_prototype then
        hull_prototype = createHullPrototype(store)
    end
    local button = scene:createBody({ x = position.x - 1, y = position.y - 1 }, button_prototype)
    local hull = scene:createBody(position, hull_prototype)
    local button_shape = button:getShape(keys.shapes.button.main)
    if button_shape then
        button_shape:setCurrentGraphics(keys.shapeGraphics.button.main)
    end
    local hull_shape = hull:getShape(keys.shapes.button.hull)
    if hull_shape then
        hull_shape:setCurrentGraphics(keys.shapeGraphics.button.hull)
    end
    scene:createPrismaticJoint({
        bodyA = hull,
        bodyB = button,
        localAxisA = { x = 0, y = -1 },
        localAnchorA = { x = 5, y = 20 },
        localAnchorB = { x = 0, y = 13 },
        isLimitEnbaled = true,
        lowerTranslation = 12,
        upperTranslation = 25,
        isMotorEnbaled = true,
        motorSpeed = 1,
        maxMotorForce = 1300
    })
    scene:createPrismaticJoint({
        bodyA = hull,
        bodyB = button,
        localAxisA = { x = 0, y = -1 },
        localAnchorA = { x = 124, y = 20 },
        localAnchorB = { x = 119, y = 13 },
        isLimitEnbaled = true,
        lowerTranslation = 12,
        upperTranslation = 25,
        isMotorEnbaled = true,
        motorSpeed = 1,
        maxMotorForce = 1300
    })
    if options then
        if options.layer then
            button:setLayer(options.layer)
            hull:setLayer(options.layer)
        end
    end
    return button
end

return {
    ---@param store sol.Store
    init = function(store)
        createButtonPrototype(store)
        createHullPrototype(store)
    end,
    createButton = createButton
}
