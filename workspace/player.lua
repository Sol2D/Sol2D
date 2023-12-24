local player = sol.body
local level = sol.level
local sprite_index = 1
local forward = true
local frames_per_sprite = 0
local next_sprite_countdown = frames_per_sprite

local start_position = level:getObjectByName('StartPosition')
player:setPosition(start_position.position.x, start_position.position.y)


local sprite_ranges = {
    idle = { 1 },
    down = { 1, 5, 9, 13 },
    up = { 3, 7, 11, 15 },
    left = { 2, 6, 10, 14 },
    right = { 4, 8, 12, 16 }
}

local range = sprite_ranges.idle

sol.heartbeat.subscribe(function ()
    local up, right, down, left, l_shift, r_shift = sol.keyboard:getKeysState(
        sol.keyboard.up_arrow,
        sol.keyboard.right_arrow,
        sol.keyboard.down_arrow,
        sol.keyboard.left_arrow,
        sol.keyboard.r_shift,
        sol.keyboard.l_shift
    )

    local step = 1
    if l_shift or r_shift then
        frames_per_sprite = 3
        step = 2
    else
        frames_per_sprite = 6
    end

    if left then
        player:move(-step, 0)
        range = sprite_ranges.left
    elseif right then
        player:move(step, 0)
        range = sprite_ranges.right
    elseif up then
        player:move(0, -step)
        range = sprite_ranges.up
    elseif down then
        player:move(0, step)
        range = sprite_ranges.down
    else
        range = sprite_ranges.idle
    end

    if sprite_index > #range then
        sprite_index = 1
    end

    player:setActiveSprite(range[sprite_index])
    next_sprite_countdown = next_sprite_countdown - 1
    if next_sprite_countdown <= 0 then
        sprite_index = sprite_index + 1
        next_sprite_countdown = frames_per_sprite
    end
end)
