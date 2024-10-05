local keys = require 'resources.keys'

---@param store sol.Store
local function initSounds(store)
    store:createSoundEffect(keys.soundEffects.armor, 'sounds/sound-effects/rpg-sound-pack/inventory/armor-light.wav')
    store:createSoundEffect(keys.soundEffects.swing, 'sounds/sound-effects/rpg-sound-pack/battle/swing.wav')

end

return {
    init = initSounds
}
