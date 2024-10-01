local keys = require 'resources.keys'

---@param store sol.Store
local function createPlatformPrototype3(store)
    local sprite_sheet = store:createSpriteSheet('flying-platform')
    if (not sprite_sheet:loadFromFile(
            'sprites/platform/platform.png',
            { colCount = 3, rowCount = 1, spriteWidth = 128, spriteHeight = 64 }
        )) then
        error('Unable to load sprites/platform/platform.png')
    end
    local platformGraphics = store:createGraphicsPack(keys.graphicPacks.flyingPlatform3.main)
    local frame = platformGraphics:addFrame()
    platformGraphics:addSprite(frame, sprite_sheet, 0, { position = { x = 0, y = 0 } })
    platformGraphics:addSprite(frame, sprite_sheet, 1, { position = { x = 128, y = 0 } })
    platformGraphics:addSprite(frame, sprite_sheet, 2, { position = { x = 256, y = 0 } })
    store:createBodyPrototype(
        keys.bodies.flyingPlatform3,
        {
            type = sol.BodyType.KINEMATIC,
            script = 'bodies/flying-platform.lua',
            shapes = {
                [keys.shapes.oneWayPlatfrom.main] = {
                    type = sol.BodyShapeType.POLYGON,
                    physics = {
                        isPreSolveEnabled = true,
                        restitution = 0.2
                    },
                    rect = { x = 0, y = 0, w = 384, h = 64 },
                    graphics = {
                        [keys.shapeGraphics.flyingPlatform3.main] = {
                            graphics = platformGraphics
                        }
                    }
                }
            }
        }
    )
end

return {
    init = createPlatformPrototype3
}
