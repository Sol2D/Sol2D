local bs, del = sol.keyboard:getState(sol.Scancode.BACKSPACE, sol.Scancode.DELETE)
print('BACKSPACE: ' .. tostring(bs) .. '\nDELETE: ' .. tostring(del))

local mouse = sol.mouse:getState()
print('LB: ' .. tostring(mouse.left) .. '\nRB: ' .. tostring(mouse.right))

local store = sol.stores:createStore('main_store')
local view = store:createView('main_view')
print('Layout: ' .. tostring(view:getLayout()))
