---@meta

---@class sol.SoundEffect
local __sound_effect

---@param channel integer?
---@return boolean
function __sound_effect:play(channel) end

---@param iteration_count integer
---@param channel integer?
---@return boolean
function __sound_effect:loop(iteration_count, channel) end

---@class sol.Music
local __music

---@return boolean
function __music:play() end

---@param iteration_count integer
---@return boolean
function __music:loop(iteration_count) end
