local scene = sol.world:createScene('main')
local larder = sol.world:createLarder('main')
scene:loadTileMap('tiled/tmx/level-01.tmx')
scene:createBodiesFromMapObjects('Obstacle')
scene:createBodiesFromMapObjects('Sensor', nil, { isSensor = true })

local function createSpacesheep()
    local body_proto = larder:createBodyPrototype('spaceship', sol.BodyType.DYNAMIC)
    local spaceship_rect = { x = 48, y = 48, w = 72, h = 96 }
    local rect_proto = body_proto:createPolygonShape('main', spaceship_rect)
    body_proto:createPolygonShape('polygon',
        { x = 25, y = 25 },
        { x = 15, y = 15 },
        { x = 35, y = 15 }
    )
    body_proto:createCircleShape('circle', { x = 15, y = 15 }, 10)
    local spacehip_sprite = larder:createSprite('spaceship')
    if spacehip_sprite:loadFromFile('assets/spaceship/spaceship.png') then
        print('Sprite loaded')
        rect_proto:addSprite('spaceship', spacehip_sprite, {
            position = { x = spaceship_rect.x, y = spaceship_rect.y },
            size = { w = spaceship_rect.w, h = spaceship_rect.h }
        })
    else
        print('Sprite loading failed')
    end
    return scene:createBody({ x = 100, y = 42 }, body_proto)
end

local function getStartPosition()
    local object = scene:getTileMapObjectByName('StartPosition')
    if object then
        return object.position
    else
        return { x = 350, y = 400 }
    end
end

local function createPlayer()
    local body_proto = larder:createBodyPrototype('player', sol.BodyType.DYNAMIC)
    body_proto:attachScript('experimental_player.lua')
    local size = { w = 32; h = 36 }
    local position = { x = -(size.w / 2), y = -(size.h / 2) }
    local shape_proto = body_proto:createPolygonShape('main', { x = position.x, y = position.y, w = size.w, h = size.h })
    local sprite_sheet = larder:createSpriteSheet('player')
    if sprite_sheet:loadFromFile('assets/player/george.png', {
        spriteWidth = size.w,
        spriteHeight = size.h,
        rowCount = 4,
        colCount = 4,
        marginTop = 8,
        marginLeft = 8,
        horizontalSpacing = 16,
        verticalSpacing = 12
    }) then
        print('Sprite sheet loaded')
    else
        print('Sprite sheet is not loaded')
    end
    local frame_duration = 200
    local animtion_idle = larder:createSpriteAnimation('player_idle') -- TODO: Load a sprite form a sheet, animation is overhead
    animtion_idle:addFrameFromSpriteSheet(0, sprite_sheet, 0)
    local animation_right = larder:createSpriteAnimation('player_right')
    animation_right:addFrames(frame_duration, sprite_sheet, { 3, 7, 11, 15 })
    local animation_left = larder:createSpriteAnimation('player_left')
    animation_left:addFrames(frame_duration, sprite_sheet, { 1, 5, 9, 13 })
    local animation_up = larder:createSpriteAnimation('player_up')
    animation_up:addFrames(frame_duration, sprite_sheet, { 2, 6, 10, 14 })
    local animation_down = larder:createSpriteAnimation('player_down')
    animation_down:addFrames(frame_duration, sprite_sheet, { 0, 4, 8, 12 })
    shape_proto:addSpriteAnimation('idle', animtion_idle, { position = position })
    shape_proto:addSpriteAnimation('right', animation_right, { position = position })
    shape_proto:addSpriteAnimation('left', animation_left, { position = position })
    shape_proto:addSpriteAnimation('up', animation_up, { position = position })
    shape_proto:addSpriteAnimation('down', animation_down, { position = position })
    return scene:createBody(getStartPosition(), body_proto)
end

-- local body_id = createSpacesheep()
local body_id = createPlayer()
scene:setBodyLayer(body_id, 'Ground')
scene:setFolowedBody(body_id)

local FORCE_MULTIPLYER = 100

local graphic = 'idle'
scene:setBodyShapeCurrentGraphic(body_id, 'main', graphic)

scene:subscribeToBeginContact(function (contact)
    print('Contact')
    print('    Body A:',  contact.sideA.bodyId)
    print('    Shape A',  contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:',  contact.sideB.bodyId)
    print('    Shape B',  contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)

    if (body_id == contact.sideA.bodyId and contact.sideB.shapeKey == 'Sensor') or
            (body_id == contact.sideB.bodyId and contact.sideA.shapeKey == 'Sensor') then
        scene:setBodyPosition(body_id, getStartPosition())
    end

end)

scene:subscribeToEndContact(function (contact)
    print('Discontact')
    print('    Body A:',  contact.sideA.bodyId)
    print('    Shape A',  contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:',  contact.sideB.bodyId)
    print('    Shape B',  contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)
end)

sol.heartbeat:subscribe(function ()
    local up, right, down, left, l_shift, r_shift = sol.keyboard:getState(
        sol.Scancode.UP_ARROW,
        sol.Scancode.RIGHT_ARROW,
        sol.Scancode.DOWN_ARROW,
        sol.Scancode.LEFT_ARROW,
        sol.Scancode.R_SHIFT,
        sol.Scancode.L_SHIFT
    )

    local new_graphic;
    if up then
        new_graphic = 'up'
    elseif down then
        new_graphic = 'down'
    elseif left then
        new_graphic = 'left'
    elseif right then
        new_graphic = 'right'
    else
        new_graphic = 'idle'
    end
    if new_graphic ~= graphic then
        scene:setBodyShapeCurrentGraphic(body_id, 'main', new_graphic)
        graphic = new_graphic
    end

    local force = { x = 0, y = 0 }
    if up then force.y = -1 end
    if right then force.x = 1 end
    if down then force.y = 1 end
    if left then force.x = -1 end
    if force.x == force.y == 0 then return end

    force.x = force.x * FORCE_MULTIPLYER
    force.y = force.y * FORCE_MULTIPLYER

    if l_shift or r_shift then
        force.x = force.x * 4
        force.y = force.y * 4
    end

    scene:applyForce(body_id, force)
end)
