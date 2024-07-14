local main_scene_name = 'main'
local scene = sol.world:createScene(main_scene_name)
local store = sol.world:createStore('main')
scene:loadTileMap('tiled/tmx/level-01.tmx')
-- scene:loadTileMap('tiled/tmx/level-03.tmx')
-- scene:loadTileMap('tiled/tmx/test2.tmx')
scene:createBodiesFromMapObjects('Obstacle')
scene:createBodiesFromMapObjects('Sensor', nil, { isSensor = true })

local main_fragment_id = sol.world:createFragment({
    top = { unit = sol.DimensionUnit.PIXEL, value = 50 }
})
sol.world:bindFragment(main_fragment_id, main_scene_name)

local font_roboto = store:createObject('sol.Font', 'roboto-18', 'fonts/Roboto/Roboto-Bold.ttf', 18)
if font_roboto == nil then
    print('Font is not loaded')
    return
end

local teleport_sound_effect = store:createObject('sol.SoundEffect', 'teleport', 'sounds/teleport/teleport.wav')
if teleport_sound_effect == nil then
    print('The teleport sound effect is not loaded')
    return
end

local click_sound_effect = store:createObject('sol.SoundEffect', 'click', 'sounds/click/click.mp3')
if click_sound_effect == nil then
    print('The click sound effect is not loaded')
    return
end

local score = {
    value = 0,
    increment = function(self)
        self.value = self.value + 1
    end,
    formatMessage = function(self)
        return string.format('SCORE: %s', self.value)
    end
}

local score_form_name = 'score'
local form = sol.world:createForm(score_form_name)

local function createScoreLabel()
    local label = form:createLabel(score:formatMessage())
    local default_fg = { r = 200, g = 255, b = 255 }
    local focused_fg = { r = 255, g = 100, b = 100 }
    local activated_fg = { r = 100, g = 255, b = 100 }
    label:setFont(font_roboto)
    label:setX('40%')
    label:setY('8px')
    label:setWidth(300);
    label:setHeight(34);
    label:setBackgroundColor({ r = 0, g = 100, b = 30 }, sol.WidgetState.FOCUSED)
    label:setForegroundColor(default_fg)
    label:setForegroundColor(focused_fg, sol.WidgetState.FOCUSED)
    label:setForegroundColor(activated_fg, sol.WidgetState.ACTIVATED)
    label:setBorderColor(default_fg)
    label:setBorderColor(focused_fg, sol.WidgetState.FOCUSED)
    label:setBorderColor(activated_fg, sol.WidgetState.ACTIVATED)
    label:setBorderWidth(6)
    label:setVerticalTextAlignment(sol.VerticalTextAlignment.CENTER)
    label:setHorizontalTextAlignment(sol.HorizontalTextAlignment.BEGIN)
    label:setPadding({ left = 15 })
    return label
end

local score_label = createScoreLabel()

local function createSwitchViewButton()
    local button = form:createButton('Switch View')
    local default_fg = { r = 200, g = 255, b = 255 }
    local focused_fg = { r = 255, g = 100, b = 100 }
    local activated_fg = { r = 100, g = 255, b = 100 }
    button:setFont(font_roboto)
    button:setX(5)
    button:setY(8)
    button:setWidth('39%');
    button:setHeight(34);
    button:setBackgroundColor({ r = 0, g = 100, b = 30 }, sol.WidgetState.FOCUSED)
    button:setForegroundColor(default_fg)
    button:setForegroundColor(focused_fg, sol.WidgetState.FOCUSED)
    button:setForegroundColor(activated_fg, sol.WidgetState.ACTIVATED)
    button:setBorderColor(default_fg)
    button:setBorderColor(focused_fg, sol.WidgetState.FOCUSED)
    button:setBorderColor(activated_fg, sol.WidgetState.ACTIVATED)
    button:setBorderWidth(6)
    button:setVerticalTextAlignment(sol.VerticalTextAlignment.CENTER)
    button:setHorizontalTextAlignment(sol.HorizontalTextAlignment.CENTER)
    button:setPadding({ left = 15 })
    return button
end

local switch_view_button = createSwitchViewButton()

local score_fragment_id = sol.world:createFragment({
    height = { unit = sol.DimensionUnit.PIXEL, value = 50 }
})
sol.world:bindFragment(score_fragment_id, score_form_name)

local function createSpacesheep()
    local body_proto = store:createObject('sol.BodyPrototype', 'spaceship', sol.BodyType.DYNAMIC)
    local spaceship_rect = { x = 48, y = 48, w = 72, h = 96 }
    local rect_proto = body_proto:createPolygonShape('main', spaceship_rect)
    body_proto:createPolygonShape('polygon',
        { x = 25, y = 25 },
        { x = 15, y = 15 },
        { x = 35, y = 15 }
    )
    body_proto:createCircleShape('circle', { x = 15, y = 15 }, 10)
    local spacehip_sprite = store:createObject('sol.Sprite', 'spaceship')
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
    local body_proto = store:createObject('sol.BodyPrototype', 'player', sol.BodyType.DYNAMIC)
    body_proto:attachScript('player.lua')
    local size = { w = 32, h = 36 }
    local position = { x = -(size.w / 2), y = -(size.h / 2) }
    local shape_proto = body_proto:createPolygonShape('main', { x = position.x, y = position.y, w = size.w, h = size.h })
    local sprite_sheet = store:createObject('sol.SpriteSheet', 'player')
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
        print('Player sprite sheet loaded')
    else
        print('Player sprite sheet is not loaded')
        return
    end
    local frame_duration = 200
    local animation_idle = store:createObject('sol.SpriteAnimation', 'player_idle') -- TODO: Load a sprite form a sheet, animation is overhead
    animation_idle:addFrameFromSpriteSheet(sprite_sheet, 0)
    local animation_right = store:createObject('sol.SpriteAnimation', 'player_right')
    animation_right:addFrames(sprite_sheet, { 3, 7, 11, 15 }, { frame_duration = frame_duration })
    local animation_left = store:createObject('sol.SpriteAnimation', 'player_left')
    animation_left:addFrames(sprite_sheet, { 1, 5, 9, 13 }, { frame_duration = frame_duration })
    local animation_up = store:createObject('sol.SpriteAnimation', 'player_up')
    animation_up:addFrames(sprite_sheet, { 2, 6, 10, 14 }, { frame_duration = frame_duration })
    local animation_down = store:createObject('sol.SpriteAnimation', 'player_down')
    animation_down:addFrames(sprite_sheet, { 0, 4, 8, 12 }, { frame_duration = frame_duration })
    shape_proto:addSpriteAnimation('idle', animation_idle, { position = position })
    shape_proto:addSpriteAnimation('right', animation_right, { position = position })
    shape_proto:addSpriteAnimation('left', animation_left, { position = position })
    shape_proto:addSpriteAnimation('up', animation_up, { position = position })
    shape_proto:addSpriteAnimation('down', animation_down, { position = position })
    return scene:createBody(getStartPosition(), body_proto)
end

local function createSkeleton()
    local body_proto = store:createObject('sol.BodyPrototype', 'skeleton', sol.BodyType.DYNAMIC)
    body_proto:attachScript('skeleton.lua')
    local size = { w = 34, h = 50 }
    local position = { x = -(size.w / 2), y = -(size.h / 2) }
    local shape_proto = body_proto:createPolygonShape('main', { x = position.x, y = position.y, w = size.w, h = size.h })
    local sprite_sheet = store:createObject('sol.SpriteSheet', 'skeleton-walkcycle')
    if sprite_sheet:loadFromFile('assets/skeleton/walkcycle.png', {
            spriteWidth = size.w,
            spriteHeight = size.h,
            rowCount = 4,
            colCount = 9,
            marginTop = 14,
            marginLeft = 15,
            horizontalSpacing = 30,
            verticalSpacing = 14
        }) then
        print('Skeleton sprite sheet loaded')
    else
        print('Skeleton sprite sheet is not loaded')
        return
    end
    local frame_duration = 80;
    local animation_idle = store:createObject('sol.SpriteAnimation', 'skeleton_idle')
    animation_idle:addFrameFromSpriteSheet(sprite_sheet, 18)
    local animation_up = store:createObject('sol.SpriteAnimation', 'skeleton_up')
    animation_up:addFrames(sprite_sheet, { 1, 2, 3, 4, 5, 6, 7, 8 }, { frame_duration = frame_duration })
    local animation_left = store:createObject('sol.SpriteAnimation', 'skeleton_left')
    animation_left:addFrames(sprite_sheet, { 10, 11, 12, 13, 14, 15, 16, 17 }, { frame_duration = frame_duration })
    local animation_down = store:createObject('sol.SpriteAnimation','skeleton_down')
    animation_down:addFrames(sprite_sheet, { 19, 20, 21, 22, 23, 24, 25, 26 }, { frame_duration = frame_duration })
    local animation_right = store:createObject('sol.SpriteAnimation','skeleton_right')
    animation_right:addFrames(sprite_sheet, { 28, 29, 30, 31, 32, 33, 34, 35 }, { frame_duration = frame_duration })
    shape_proto:addSpriteAnimation('idle', animation_idle, { position = position })
    shape_proto:addSpriteAnimation('right', animation_right, { position = position })
    shape_proto:addSpriteAnimation('left', animation_left, { position = position })
    shape_proto:addSpriteAnimation('up', animation_up, { position = position })
    shape_proto:addSpriteAnimation('down', animation_down, { position = position })
    -- scene:createBody(nil, body_proto, { trackName = 'SkeletonTrack', startPoint = 2 })
    -- scene:createBody(nil, body_proto, { trackName = 'SkeletonTrack', startPoint = 3 })
    -- scene:createBody(nil, body_proto, { trackName = 'SkeletonTrack', startPoint = 4 })
    return scene:createBody(nil, body_proto, { trackName = 'SkeletonTrack', startPoint = 1 })
end

(function ()
    local music = store:createObject('sol.Music', 'village-in-despair', 'sounds/village_in_despair/village_in_despair.flac')
    if music == nil then
        print('Unable to load music')
    else
        music:loop(-1)
    end
end)()

-- local body_id = createSpacesheep()
local player_body_id = createPlayer()
local skeleton_body_id = createSkeleton()
if player_body_id == nil or skeleton_body_id == nil then
    return;
end
scene:setBodyLayer(player_body_id, 'Ground')
scene:setBodyLayer(skeleton_body_id, 'Ground')
local followed_body_id = player_body_id;
scene:setFollowedBody(followed_body_id)

local FORCE_MULTIPLYER = 100

local graphic = 'idle'
scene:setBodyShapeCurrentGraphic(player_body_id, 'main', graphic)
scene:setBodyShapeCurrentGraphic(skeleton_body_id, 'main', graphic)

scene:subscribeToBeginContact(function(contact)
    print('Contact')
    print('    Body A:', contact.sideA.bodyId)
    print('    Shape A', contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:', contact.sideB.bodyId)
    print('    Shape B', contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)

    if (player_body_id == contact.sideA.bodyId and contact.sideB.shapeKey == 'Sensor') or
        (player_body_id == contact.sideB.bodyId and contact.sideA.shapeKey == 'Sensor') then
        teleport_sound_effect:play()
        scene:setBodyPosition(player_body_id, getStartPosition())
    else
        score:increment()
        score_label:setText(score:formatMessage())
    end
end)

scene:subscribeToEndContact(function(contact)
    print('Discontact')
    print('    Body A:', contact.sideA.bodyId)
    print('    Shape A', contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:', contact.sideB.bodyId)
    print('    Shape B', contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)
end)


switch_view_button:subscribeOnClick(function()
    if followed_body_id == player_body_id then
        followed_body_id = skeleton_body_id
    else
        followed_body_id = player_body_id
    end
    scene:setFollowedBody(followed_body_id)
    click_sound_effect:play()
end)

sol.heartbeat:subscribe(function()
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
        scene:setBodyShapeCurrentGraphic(player_body_id, 'main', new_graphic)
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

    scene:applyForce(player_body_id, force)
end)
