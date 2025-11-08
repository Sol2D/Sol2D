local Game = {}

local game = nil

--@param store sol.Store
--@return Game
function Game.new(store)
    if game then
        error('A game already created')
    end

    local main_track_key = 'game-main-track'

    game = {}
    game.__index = game
    game.main_track = store:createAudioTrack(main_track_key)
    game.main_track:setLoop(-1)

    --@param audio sol.Audio
    function game.playMusic(audio)
        game.main_track:setAudio(audio)
        game.main_track:play()
    end

    return setmetatable({}, game)
end

return Game

