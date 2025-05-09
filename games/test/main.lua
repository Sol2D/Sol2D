local bs, del = sol.keyboard:getState(sol.Scancode.BACKSPACE, sol.Scancode.DELETE)
print('BACKSPACE: ' .. tostring(bs) .. '\nDELETE: ' .. tostring(del))

local mouse = sol.mouse:getState()
print('LB: ' .. tostring(mouse.left) .. '\nRB: ' .. tostring(mouse.right))
