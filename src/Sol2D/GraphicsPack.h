// Sol2D Game Engine
// Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
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

#include <Sol2D/SpriteSheet.h>
#include <Sol2D/GraphicsPackDefinition.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <chrono>
#include <vector>

namespace Sol2D {

class GraphicsPack final
{
private:
    struct Graphics;
    struct Frame;

public:
    explicit GraphicsPack(Renderer & _renderer, const GraphicsPackDefinition & _definition = GraphicsPackDefinition());
    GraphicsPack(const GraphicsPack & _graphics_pack);
    GraphicsPack(GraphicsPack && _graphics_pack);
    ~GraphicsPack();
    GraphicsPack & operator= (const GraphicsPack & _graphics_pack);
    GraphicsPack & operator= (GraphicsPack && _graphics_pack);

    void setFilippedHorizontally(bool _flipped);
    void setFilippedVertically(bool _flipped);
    void setCenter(const SDL_FPoint & _center);
    size_t addFrame(const GraphicsPackFrameDefinition & _definition);
    size_t insertFrame(size_t _index, const GraphicsPackFrameDefinition & _definition);
    bool removeFrame(size_t _index);
    bool setFrameVisibility(size_t _index, bool _is_visible);
    std::optional<bool> isFrameVisible(size_t _index) const;
    bool setFrameDuration(size_t _index, std::chrono::milliseconds _duration);
    std::optional<std::chrono::milliseconds> getFrameDuration(size_t _index) const;
    bool setCurrentFrameIndex(size_t _index);
    size_t getCurrentFrameIndex() const;
    bool switchToFirstVisibleFrame();
    bool switchToNextVisibleFrame();
    size_t getCurrentAnimationIteration() const;
    std::pair<bool, size_t> addSprite(size_t _frame, const GraphicsPackSpriteDefinition & _definition);
    bool removeSprite(size_t _frame, size_t _sprite);
    void render(const SDL_FPoint & _position, const Rotation & _rotation, std::chrono::milliseconds _delta_time);

private:
    bool switchToNextVisibleFrame(bool _respect_iteration);
    void destroy();
    void performRender(const SDL_FPoint & _position, const Rotation & _rotation);

private:
    Renderer * m_renderer;
    SDL_FPoint m_position;
    SDL_FlipMode m_flip_mode;
    std::vector<Frame *> m_frames;
    int32_t m_max_iterations;
    int32_t m_current_iteration;
    size_t m_current_frame_index;
    std::chrono::milliseconds m_current_frame_duration;
    std::chrono::milliseconds m_total_duration;
};

inline size_t GraphicsPack::getCurrentFrameIndex() const
{
    return m_current_frame_index;
}

inline void GraphicsPack::setFilippedHorizontally(bool _flipped)
{
    m_flip_mode = _flipped ? static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) | SDL_FLIP_HORIZONTAL)
                           : static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) & ~SDL_FLIP_HORIZONTAL);
}

inline void GraphicsPack::setFilippedVertically(bool _flipped)
{
    m_flip_mode = _flipped ? static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) | SDL_FLIP_VERTICAL)
                           : static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) & ~SDL_FLIP_VERTICAL);
}

inline size_t GraphicsPack::getCurrentAnimationIteration() const
{
    return m_current_iteration;
}

} // namespace Sol2D
