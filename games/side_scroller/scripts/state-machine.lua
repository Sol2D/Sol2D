---@class StateMachineTransition
---@field source any
---@field target any
---@field event any
---@field canEnter fun(args: any | nil)?: boolean
---@field canLeave fun()?: boolean
---@field onEnter fun(args: any | nil)?
---@field onLeave function?

local module = {}
module.__index = module

---@param initial_state any
---@param transitions StateMachineTransition[]
local function createStateMachine(initial_state, transitions)
    local current_state = initial_state
    local machine = {}

    ---@param event any
    ---@param args any?
    function machine.processEvent(event, args)
        for _, transition in ipairs(transitions) do
            if
                transition.event == event and
                transition.source == current_state and
                (not transition.canLeave or transition.canLeave()) and
                (not transition.canEnter or transition.canEnter(args))
            then
                if transition.onLeave then
                    transition.onLeave()
                end
                if transition.onEnter then
                    transition.onEnter(args)
                end
                current_state = transition.target

                print('State changed: ' .. transition.source .. ' -> ' .. transition.target)

                return true
            end
        end
        return false
    end

    return machine
end

---@param initial_state any
---@param transitions StateMachineTransition[]
function module.new(initial_state, transitions)
    return createStateMachine(initial_state, transitions)
end

return setmetatable({}, module)
