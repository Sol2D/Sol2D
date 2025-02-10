local StateMachine = {}
StateMachine.__index = StateMachine

---@alias StateMachineStateKey string
---@alias StateMachineEvent integer

---@class StateMachineState
---@field canEnter? fun(): boolean
---@field enter? fun(event: StateMachineEvent)
---@field update? fun(dt?: integer)
---@field canLeave? fun(): boolean
---@field leave? fun()

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

    ---@param event StateMachineEvent
    function sm.processEvent(event)
        if current.state.canLeave and not current.state.canLeave() then
            return
        end
        for _, tran in ipairs(transitions) do
            if tran.event == event and tran.from == current.key then
                local target = states[tran.to]
                if target and (not target.canEnter or target.canEnter()) then
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

    ---@param dt integer
    function sm.update(dt)
        if current.state.update then
            current.state.update(dt)
        end
    end

    return sm
end

return setmetatable({}, StateMachine)
