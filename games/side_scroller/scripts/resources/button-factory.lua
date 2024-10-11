local keys = require 'resources.keys'

local function getSprite(store, key, file)
    local sprite = store:getSprite(key)
    if not sprite then
        sprite = store:createSprite(key)
        if not sprite:loadFromFile(file) then
            error('Unable to load ' .. file)
        end
    end
    return sprite
end

---@param store sol.Store
---@param scene sol.Scene
---@param position Point
---@param options { layer: string }?
local function createButton(store, scene, position, options)
    local button_sprite = getSprite(store, 'button', 'sprites/button/button.png')
    local hull_sprite = getSprite(store, 'button-hull', 'sprites/button/hull.png')
    local button = scene:createBody(
        { x = position.x - 1, y = position.y - 1 },
        {
            type = sol.BodyType.DYNAMIC,
            physics = { fixedRotation = true },
            shapes = {
                [keys.shapes.button.main] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        density = 100
                    },
                    rect = { x = 0, y = 0, w = 119, h = 26 },
                    graphics = {
                        [keys.shapeGraphics.button.main] = {
                            animationIterations = 0,
                            frames = { { sprites = { { sprite = button_sprite } } } }
                        }
                    }
                }
            }
        }
    )
    local hull = scene:createBody(
        position,
        {
            type = sol.BodyType.STATIC,
            shapes = {
                [keys.shapes.button.hull] = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = 0, y = 0, w = 129, h = 40 },
                    graphics = {
                        [keys.shapeGraphics.button.hull] = {
                            animationIterations = 0,
                            frames = { { sprites = { { sprite = hull_sprite } } } }
                        }
                    }
                }
            }
        }
    )
    scene:createBody(
        -- { x = position.x + 10, y = position.y + 15 },
        { x = position.x + 0.15, y = position.y + 0.15 },
        {
            type = sol.BodyType.STATIC,
            shapes = {
                [keys.shapes.button.sensor] = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = 0, y = 0, w = 99, h = 5 },
                    physics = {
                        isSensor = true
                    }
                }
            }
        }
    )
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
    createButton = createButton
}
