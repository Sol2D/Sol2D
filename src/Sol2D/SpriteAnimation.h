// Sol2D Game Engine
// Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
//
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <Sol2D/Sprite.h>
#include <Sol2D/SpriteSheet.h>
#include <Sol2D/SpriteRenderOptions.h>
#include <chrono>
#include <vector>

namespace Sol2D {

class SpriteAnimation final
{
    struct Frame;

public:
    explicit SpriteAnimation(SDL_Renderer & _renderer);
    SpriteAnimation(const SpriteAnimation & _animation);
    SpriteAnimation(SpriteAnimation && _animation);
    ~SpriteAnimation();
    SpriteAnimation & operator = (const SpriteAnimation & _animation);
    SpriteAnimation & operator = (SpriteAnimation && _animation);
    void clear();
    bool addFrame(std::chrono::milliseconds _duration, const Sprite & _sprite);
    bool addFrame(std::chrono::milliseconds _duration, const SpriteSheet & _sprite_sheet, size_t _sprite_index);
    bool addFrames(std::chrono::milliseconds _duration,
        const SpriteSheet & _sprite_sheet,
        std::vector<size_t> _sprite_indices);
    void render(
        const SDL_FPoint & _point,
        std::chrono::milliseconds _time_passed,
        SpriteRenderOptions _options = SpriteRenderOptions());

private:
    static void copy(const SpriteAnimation & _from, SpriteAnimation & _to);

private:
    SDL_Renderer * mp_renderer;
    std::vector<Frame *> m_frames;
    size_t m_current_frame_index;
    std::chrono::milliseconds m_current_frame_duration;
    std::chrono::milliseconds m_total_duration;
};

} // namespace Sol2D
