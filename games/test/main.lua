local bs, del = sol.keyboard:getState(sol.Scancode.BACKSPACE, sol.Scancode.DELETE)
print('BACKSPACE: ' .. tostring(bs) .. '\nDELETE: ' .. tostring(del))

local mouse = sol.mouse:getState()
print('LB: ' .. tostring(mouse.left) .. '\nRB: ' .. tostring(mouse.right))

local store = sol.stores:createStore('main_store')
local view = store:createView('main_view')
local layout = view:getLayout()

print('sol.PositionUnit.POINT: ' .. tostring(sol.PositionUnit.POINT))

local node = layout:addNode({
    minHeight = { value = 50, unit = sol.DimensionLimitUnit.POINT },
    minWidth = { value = 50, unit = sol.DimensionLimitUnit.POINT },
    -- position = {
    --     left = { value = 10, unit = sol.PositionUnit.POINT },
    --     top = { value = 10, unit = sol.PositionUnit.POINT }
    -- },
    -- positionType = sol.PositionType.ABSOLUTE
    flexGrow = 2
})

local scene = store:createScene('scene', node)
scene:loadTileMap('/home/brainstream/projects/sol2d/build/Desktop-Debug/games/rpg/tiled/tmx/level-01.tmx')

sol.window:setView(view)