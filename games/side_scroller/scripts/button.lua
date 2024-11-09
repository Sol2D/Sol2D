local resources = require 'resources'

local keys = {
    shapes = {
        CAP = 'cap',
        HULL = 'hull',
        SENSOR = 'sensor'
    },
    shapeGraphics = {
        CAP = 'cap',
        HULL = 'hull'
    }
}

local module = {}
module.__index = module
module.keys = keys

---@param scene sol.Scene
---@param position sol.Point
---@param options { layer: string }?
function module.new(scene, position, options)
    local button = scene:createBody(
        { x = position.x - 1, y = position.y - 1 },
        {
            type = sol.BodyType.DYNAMIC,
            physics = { fixedRotation = true },
            shapes = {
                [keys.shapes.CAP] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        density = 100
                    },
                    rect = { x = 0, y = 0, w = 119, h = 26 },
                    graphics = {
                        [keys.shapeGraphics.CAP] = {
                            animationIterations = 0,
                            frames = {
                                {
                                    sprites = {
                                        {
                                            sprite = resources.getSprite(resources.keys.sprites.button.CAP)
                                        }
                                    }
                                }
                            }
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
                [keys.shapes.HULL] = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = 0, y = 0, w = 129, h = 40 },
                    graphics = {
                        [keys.shapeGraphics.HULL] = {
                            animationIterations = 0,
                            frames = {
                                {
                                    sprites = {
                                        {
                                            sprite = resources.getSprite(resources.keys.sprites.button.HULL)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    )
    scene:createBody(
        { x = position.x + 0.15, y = position.y + 0.15 },
        {
            type = sol.BodyType.STATIC,
            shapes = {
                [keys.shapes.SENSOR] = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = 0, y = 0, w = 99, h = 5 },
                    physics = {
                        isSensor = true
                    }
                }
            }
        }
    )
    local button_shape = button:getShape(keys.shapes.CAP)
    if button_shape then
        button_shape:setCurrentGraphics(keys.shapeGraphics.CAP)
    end
    local hull_shape = hull:getShape(keys.shapes.HULL)
    if hull_shape then
        hull_shape:setCurrentGraphics(keys.shapeGraphics.HULL)
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

return setmetatable({}, module)
