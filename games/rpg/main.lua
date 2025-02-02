require 'def'

local store = sol.stores:createStore('main')
local view = store:createView('main')
local scene = store:createScene('level-01', { metersPerPixel = Def.metersPerPixel })

scene:loadTileMap('tiled/tmx/level-01.tmx')
-- scene:loadTileMap('tiled/tmx/level-03.tmx')
-- scene:loadTileMap('tiled/tmx/test2.tmx')
scene:createBodiesFromMapObjects('Obstacle')
scene:createBodiesFromMapObjects(
    'Sensor',
    {
        type = sol.BodyType.STATIC,
        shapePhysics = {
            isSensor = true
        }
    }
)

local main_fragment_id = view:createFragment({
    top = { unit = sol.DimensionUnit.PIXEL, value = 50 }
})
view:bindFragment(main_fragment_id, scene)

local font_roboto = store:createFont('roboto-18', 'fonts/Roboto/Roboto-Bold.ttf', 18)
if font_roboto == nil then
    print('Font is not loaded')
    return
end

local teleport_sound_effect = store:createSoundEffect('teleport', 'sounds/teleport/teleport.wav')
if teleport_sound_effect == nil then
    print('The teleport sound effect is not loaded')
    return
end

local click_sound_effect = store:createSoundEffect('click', 'sounds/click/click.mp3')
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

local form = store:createForm('score')
form:setBackgroundColor({ r = .24, g = .145, b = 0.1 })

local function createScoreLabel()
    local label = form:createLabel(score:formatMessage())
    local default_fg = { r = 0.78, g = 1.0, b = 1.0 }
    local focused_fg = { r = 1.0, g = 0.39, b = 0.39 }
    local activated_fg = { r = 0.39, g = 1.0, b = 0.39 }
    label:setFont(font_roboto)
    label:setX('40%')
    label:setY('8px')
    label:setWidth(300);
    label:setHeight(34);
    label:setBackgroundColor({ r = 0, g = 0.39, b = 0.12 }, sol.WidgetState.FOCUSED)
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
    local default_fg = { r = 0.78, g = 1.0, b = 1.0 }
    local focused_fg = { r = 1.0, g = 0.39, b = 0.39 }
    local activated_fg = { r = 0.39, g = 1.0, b = 0.39 }
    button:setFont(font_roboto)
    button:setX(5)
    button:setY(8)
    button:setWidth('39%');
    button:setHeight(34);
    button:setBackgroundColor({ r = 0, g = 0.39, b = 0.12 }, sol.WidgetState.FOCUSED)
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
        return Def.pixelPointToPhisical(object.position)
    else
        return Def.pixelPointToPhisical({ x = 350, y = 400 })
    end
end

local function createPlayer()
    local size = { w = 32, h = 36 }
    local sprite_sheet = store:createSpriteSheet('player')
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
    local position = { x = -(size.w / 2), y = -(size.h / 2) }
    return scene:createBody(
        getStartPosition(),
        {
            type = sol.BodyType.DYNAMIC,
            physics = {
                linearDamping = 100,
                fixedRotation = true
            },
            shapes = {
                main = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = position.x, y = position.y, w = size.w, h = size.h },
                    graphics = {
                        idle = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 0 } } }
                                }
                            }
                        },
                        right = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 3 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 7 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 11 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 15 } } }
                                }
                            }
                        },
                        left = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 1 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 5 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 9 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 13 } } }
                                }
                            }
                        },
                        down = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 0 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 4 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 8 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 12 } } }
                                }
                            }
                        },
                        up = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 2 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 6 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 10 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 14 } } }
                                },
                            }
                        }
                    }
                }
            }
        }
    )
end

local function createSkeleton()
    local size = { w = 34, h = 50 }
    local sprite_sheet = store:createSpriteSheet('skeleton-walkcycle')
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
    local position = { x = -(size.w / 2), y = -(size.h / 2) }
    return scene:createBody(
        nil,
        {
            type = sol.BodyType.DYNAMIC,
            physics = {
                fixedRotation = true,
                linearDamping = 100
            },
            shapes = {
                main = {
                    type = sol.BodyShapeType.POLYGON,
                    rect = { x = position.x, y = position.y, w = size.w, h = size.h },
                    graphics = {
                        idle = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = {
                                        { sprite = { spriteSheet = sprite_sheet, spriteIndex = 18 } },
                                    }
                                }
                            }
                        },
                        right = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 28 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 29 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 30 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 31 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 32 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 33 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 34 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 35 } } }
                                }
                            }
                        },
                        left = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 10 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 11 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 12 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 13 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 14 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 15 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 16 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 17 } } }
                                }
                            }
                        },
                        up = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 1 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 2 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 3 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 4 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 5 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 6 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 7 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 8 } } }
                                }
                            }
                        },
                        down = {
                            position = position,
                            frames = {
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 19 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 20 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 21 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 22 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 23 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 24 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 25 } } }
                                },
                                {
                                    duration = frame_duration,
                                    sprites = { { sprite = { spriteSheet = sprite_sheet, spriteIndex = 26 } } }
                                }
                            }
                        }
                    }
                }
            }
        },
        'skeleton.lua',
        {
            trackName = 'SkeletonTrack',
            startPoint = 1
        }
    )
end

(function()
    local music = store:createMusic('village-сin-despair', 'sounds/village_in_despair/village_in_despair.flac')
    if music == nil then
        print('Unable to load music')
    else
        music:loop(-1)
    end
end)()

local player = createPlayer()
local skeleton = createSkeleton()

if not player or not skeleton then
    return;
end
local player_main_shape = player:getShape('main')
local skeleton_main_shape = skeleton:getShape('main')
if not player_main_shape or not skeleton_main_shape then
    return;
end
local player_body_id = player:getId()
player:setLayer('Ground')
skeleton:setLayer('Ground')

local followed_body = player;
scene:setFollowedBody(followed_body)

local graphic = 'idle'

player_main_shape:setCurrentGraphics(graphic)
skeleton_main_shape:setCurrentGraphics(graphic)

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

scene:subscribeToSensorBeginContact(function(contact)
    if player_body_id == contact.visitor.bodyId and contact.sensor.shapeKey == 'Sensor' then
        teleport_sound_effect:play()
        player:setPosition(getStartPosition())
    end
end)

switch_view_button:subscribeOnClick(function()
    if followed_body == player then
        followed_body = skeleton
    else
        followed_body = player
    end
    scene:setFollowedBody(followed_body)
    click_sound_effect:play()
end)

local PLAYER_WALK_FORCE = 1700
scene:subscribeToStep(function()
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
        player_main_shape:setCurrentGraphics(new_graphic)
        graphic = new_graphic
    end

    local force = { x = 0, y = 0 }
    if up then force.y = -PLAYER_WALK_FORCE end
    if right then force.x = PLAYER_WALK_FORCE end
    if down then force.y = PLAYER_WALK_FORCE end
    if left then force.x = -PLAYER_WALK_FORCE end
    if force.x == force.y == 0 then return end

    if l_shift or r_shift then
        force.x = force.x * 3
        force.y = force.y * 3
    end

    player:applyForceToCenter(force)
end)
