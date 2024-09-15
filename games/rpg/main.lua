local store = sol.stores:createStore('main')
local view = store:createObject('sol.View', 'main')
local scene = store:createObject('sol.Scene', 'level-01', { scaleFactor = 200 })

scene:loadTileMap('tiled/tmx/level-01.tmx')
-- scene:loadTileMap('tiled/tmx/level-03.tmx')
-- scene:loadTileMap('tiled/tmx/test2.tmx')
scene:createBodiesFromMapObjects('Obstacle')
scene:createBodiesFromMapObjects('Sensor', nil, { isSensor = true })

local main_fragment_id = view:createFragment({
    top = { unit = sol.DimensionUnit.PIXEL, value = 50 }
})
view:bindFragment(main_fragment_id, scene)

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

local form = store:createObject('sol.Form', 'score')

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

local score_fragment_id = view:createFragment({
    height = { unit = sol.DimensionUnit.PIXEL, value = 50 }
})
view:bindFragment(score_fragment_id, form)

sol.window:setView(view)

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

    local graphics_idle = store:createObject('sol.GraphicsPack', 'player_idle')
    graphics_idle:addFrame()
    graphics_idle:addSprite(0, sprite_sheet, 0)

    local graphics_right = store:createObject('sol.GraphicsPack', 'player_right')
    graphics_right:addFrames(4, { duration = frame_duration })
    graphics_right:addSprite(0, sprite_sheet, 3)
    graphics_right:addSprite(1, sprite_sheet, 7)
    graphics_right:addSprite(2, sprite_sheet, 11)
    graphics_right:addSprite(3, sprite_sheet, 15)

    local graphics_left = store:createObject('sol.GraphicsPack', 'player_left')
    graphics_left:addFrames(4, { duration = frame_duration })
    graphics_left:addSprite(0, sprite_sheet, 1)
    graphics_left:addSprite(1, sprite_sheet, 5)
    graphics_left:addSprite(2, sprite_sheet, 9)
    graphics_left:addSprite(3, sprite_sheet, 13)

    local graphics_up = store:createObject('sol.GraphicsPack', 'player_up')
    graphics_up:addFrames(4, { duration = frame_duration })
    graphics_up:addSprite(0, sprite_sheet, 2)
    graphics_up:addSprite(1, sprite_sheet, 6)
    graphics_up:addSprite(2, sprite_sheet, 10)
    graphics_up:addSprite(3, sprite_sheet, 14)

    local graphics_down = store:createObject('sol.GraphicsPack', 'player_down')
    graphics_down:addFrames(4, { duration = frame_duration })
    graphics_down:addSprite(0, sprite_sheet, 0)
    graphics_down:addSprite(1, sprite_sheet, 4)
    graphics_down:addSprite(2, sprite_sheet, 8)
    graphics_down:addSprite(3, sprite_sheet, 12)

    shape_proto:addGraphics('idle', graphics_idle, { position = position })
    shape_proto:addGraphics('right', graphics_right, { position = position })
    shape_proto:addGraphics('left', graphics_left, { position = position })
    shape_proto:addGraphics('up', graphics_up, { position = position })
    shape_proto:addGraphics('down', graphics_down, { position = position })
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

    local graphics_idle = store:createObject('sol.GraphicsPack', 'skeleton_idle')
    graphics_idle:addFrame()
    graphics_idle:addSprite(0, sprite_sheet, 18)

    local graphics_up = store:createObject('sol.GraphicsPack', 'skeleton_up')
    graphics_up:addFrames(8, { duration = frame_duration })
    graphics_up:addSprite(0, sprite_sheet, 1)
    graphics_up:addSprite(1, sprite_sheet, 2)
    graphics_up:addSprite(2, sprite_sheet, 3)
    graphics_up:addSprite(3, sprite_sheet, 4)
    graphics_up:addSprite(4, sprite_sheet, 5)
    graphics_up:addSprite(5, sprite_sheet, 6)
    graphics_up:addSprite(6, sprite_sheet, 7)
    graphics_up:addSprite(7, sprite_sheet, 8)

    local graphics_left = store:createObject('sol.GraphicsPack', 'skeleton_left')
    graphics_left:addFrames(8, { duration = frame_duration })
    graphics_left:addSprite(0, sprite_sheet, 10)
    graphics_left:addSprite(1, sprite_sheet, 11)
    graphics_left:addSprite(2, sprite_sheet, 12)
    graphics_left:addSprite(3, sprite_sheet, 13)
    graphics_left:addSprite(4, sprite_sheet, 14)
    graphics_left:addSprite(5, sprite_sheet, 15)
    graphics_left:addSprite(6, sprite_sheet, 16)
    graphics_left:addSprite(7, sprite_sheet, 17)

    local graphics_down = store:createObject('sol.GraphicsPack', 'skeleton_down')
    graphics_down:addFrames(8, { duration = frame_duration })
    graphics_down:addSprite(0, sprite_sheet, 19)
    graphics_down:addSprite(1, sprite_sheet, 20)
    graphics_down:addSprite(2, sprite_sheet, 21)
    graphics_down:addSprite(3, sprite_sheet, 22)
    graphics_down:addSprite(4, sprite_sheet, 23)
    graphics_down:addSprite(5, sprite_sheet, 24)
    graphics_down:addSprite(6, sprite_sheet, 25)
    graphics_down:addSprite(7, sprite_sheet, 26)

    local graphics_right = store:createObject('sol.GraphicsPack', 'skeleton_right')
    graphics_right:addFrames(8, { duration = frame_duration })
    graphics_right:addSprite(0, sprite_sheet, 28)
    graphics_right:addSprite(1, sprite_sheet, 29)
    graphics_right:addSprite(2, sprite_sheet, 30)
    graphics_right:addSprite(3, sprite_sheet, 31)
    graphics_right:addSprite(4, sprite_sheet, 32)
    graphics_right:addSprite(5, sprite_sheet, 33)
    graphics_right:addSprite(6, sprite_sheet, 34)
    graphics_right:addSprite(7, sprite_sheet, 35)

    shape_proto:addGraphics('idle', graphics_idle, { position = position })
    shape_proto:addGraphics('right', graphics_right, { position = position })
    shape_proto:addGraphics('left', graphics_left, { position = position })
    shape_proto:addGraphics('up', graphics_up, { position = position })
    shape_proto:addGraphics('down', graphics_down, { position = position })
    return scene:createBody(nil, body_proto, { trackName = 'SkeletonTrack', startPoint = 1 })
end

(function ()
    local music = store:createObject('sol.Music', 'village-сin-despair', 'sounds/village_in_despair/village_in_despair.flac')
    if music == nil then
        print('Unable to load music')
    else
        music:loop(-1)
    end
end)()

local player_body_id = createPlayer()
local skeleton_body_id = createSkeleton()
if player_body_id == nil or skeleton_body_id == nil then
    return;
end
scene:setBodyLayer(player_body_id, 'Ground')
scene:setBodyLayer(skeleton_body_id, 'Ground')
local followed_body_id = player_body_id;
scene:setFollowedBody(followed_body_id)

local graphic = 'idle'
scene:setBodyShapeCurrentGraphic(player_body_id, 'main', graphic)
scene:setBodyShapeCurrentGraphic(skeleton_body_id, 'main', graphic)

scene:subscribeToBeginContact(function(contact)
    print('Begin Contact')
    print('    Body A:', contact.sideA.bodyId)
    print('    Shape A', contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:', contact.sideB.bodyId)
    print('    Shape B', contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)
    score:increment()
    score_label:setText(score:formatMessage())
end)

scene:subscribeToEndContact(function(contact)
    print('End Contact')
    print('    Body A:', contact.sideA.bodyId)
    print('    Shape A', contact.sideA.shapeKey)
    print('    Object A', contact.sideA.tileMapObjectId)
    print('    Body B:', contact.sideB.bodyId)
    print('    Shape B', contact.sideB.shapeKey)
    print('    Object B', contact.sideB.tileMapObjectId)
end)

scene:subscribeToBeginSensorContact(function (contact)
    if player_body_id == contact.visitor.bodyId and contact.sensor.shapeKey == 'Sensor' then
        teleport_sound_effect:play()
        scene:setBodyPosition(player_body_id, getStartPosition())
    end
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

    if l_shift or r_shift then
        force.x = force.x * 4
        force.y = force.y * 4
    end

    scene:applyForce(player_body_id, force)
end)
