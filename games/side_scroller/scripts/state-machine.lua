local StateMachine = {}
StateMachine.__index = StateMachine

---@alias StateMachineStateKey string
---@alias StateMachineEvent integer

---@class StateMachineState
---@field canEnter? fun(): boolean
---@field enter? fun(event: StateMachineEvent)
---@field canLeave? fun(): boolean
---@field leave? fun()
---@field update? fun(data: any)

---@class StateMachineTransition
---@field from StateMachineStateKey
---@field event StateMachineEvent
---@field to StateMachineStateKey

---@param states { [StateMachineStateKey]: StateMachineState }
---@param initial_state StateMachineStateKey
---@param transitions  StateMachineTransition[]
function StateMachine.new(states, initial_state, transitions)
    local current = {
        key = initial_state,
        state = states[initial_state]
    }

    local sm = {}
    sm.__index = sm

    ---@param event StateMachineEvent
    function sm.processEvent(event)
        if current.state.canLeave and not current.state.canLeave() then
            return
        end
        for _, tran in ipairs(transitions) do
            if tran.event == event and tran.from == current.key then
                local target = states[tran.to]
                if target and (not target.canEnter or target.canEnter()) then
                    if current.state.leave then
                        current.state.leave()
                    end
                    current.key = tran.to
                    current.state = target
                    if target.enter then
                        target.enter(event)
                    end
                    break
                end
            end
        end
    end

    function sm.getCurrentState()
        return current.key
    end

    ---@param data any
    function sm.update(data)
        if current.state and current.state.update then
            current.state.update(data)
        end
    end

    return sm
end

return setmetatable({}, StateMachine)
