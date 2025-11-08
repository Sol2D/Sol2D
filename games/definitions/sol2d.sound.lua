---@meta

---@class sol.AudioTrack
local __audio_track

---@return boolean
function __audio_track:play() end

---@param iteration_count integer
---@return boolean
function __audio_track:setLoop(iteration_count) end

---@param audio sol.Audio | nil
---@return boolean
function __audio_track:setAudio(audio) end

---@class sol.Audio
local __audio

---@return boolean
function __audio:play() end

