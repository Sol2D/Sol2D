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

#include <Sol2D/SpriteSheet.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <chrono>
#include <vector>

namespace Sol2D {

struct GraphicsPackFrameOptions
{
    GraphicsPackFrameOptions() :
        duration(std::chrono::milliseconds::zero()),
        is_visible(true)
    {
    }

    std::chrono::milliseconds duration;
    bool is_visible;
};

struct GraphicsPackSpriteOptions
{
    GraphicsPackSpriteOptions() :
        is_visible(true),
        position{.0f, .0f}
    {
    }

    bool is_visible;
    Point position;
};

class GraphicsPack final
{
    struct Graphics
    {
        S2_DEFAULT_COPY_AND_MOVE(Graphics)

        Graphics(const Sprite & _sprite, const GraphicsPackSpriteOptions & _options) :
            sprite(_sprite),
            is_visible(_options.is_visible),
            position(_options.position)
        {
        }

        Graphics(Sprite && _sprite, const GraphicsPackSpriteOptions & _options) :
            sprite(std::move(_sprite)),
            is_visible(_options.is_visible),
            position(_options.position)
        {
        }

        Sprite sprite;
        bool is_visible;
        Point position;
    };

    struct Frame
    {
        S2_DEFAULT_COPY_AND_MOVE(Frame)

        Frame(const GraphicsPackFrameOptions & _options) :
            duration(_options.duration),
            is_visible(_options.is_visible)
        {
        }

        std::chrono::milliseconds duration;
        bool is_visible;
        std::vector<Graphics> graphics;
    };

public:
    explicit GraphicsPack(SDL_Renderer & _renderer);
    GraphicsPack(const GraphicsPack & _graphics_pack);
    GraphicsPack(GraphicsPack && _graphics_pack);
    ~GraphicsPack();
    GraphicsPack & operator = (const GraphicsPack & _graphics_pack);
    GraphicsPack & operator = (GraphicsPack && _graphics_pack);

    size_t addFrame(const GraphicsPackFrameOptions & _options);
    size_t addFrames(size_t _count, const GraphicsPackFrameOptions & _options);
    size_t insertFrame(size_t _index, const GraphicsPackFrameOptions & _options);
    bool removeFrame(size_t _index);
    bool setFrameVisibility(size_t _index, bool _is_visible);
    std::optional<bool> isFrameVisible(size_t _index) const;
    bool setFrameDuration(size_t _index, std::chrono::milliseconds _duration);
    std::optional<std::chrono::milliseconds> getFrameDuration(size_t _index) const;
    std::pair<bool, size_t> addSprite(
        size_t _frame,
        const Sprite & _sprite,
        const GraphicsPackSpriteOptions & _options);
    std::pair<bool, size_t> addSprite(
        size_t _frame,
        const SpriteSheet & _sprite_sheet,
        size_t _sprite_index,
        const GraphicsPackSpriteOptions & _options);
    std::pair<bool, size_t> addSprites(
        size_t _frame,
        const SpriteSheet & _sprite_sheet,
        std::vector<size_t> _sprite_indices,
        const GraphicsPackSpriteOptions & _options);
    bool removeSprite(size_t _frame, size_t _sprite);
    void render(
        const Point & _position,
        std::chrono::milliseconds _time_passed,
        const GraphicsRenderOptions & _options = GraphicsRenderOptions());

private:
    void destroy();
    void performRender(const Point & _position, const GraphicsRenderOptions & _options);
    Frame * switchToNextVisibleFrame();

private:
    SDL_Renderer * mp_renderer;
    std::vector<Frame *> m_frames;
    size_t m_current_frame_index;
    std::chrono::milliseconds m_current_frame_duration;
    std::chrono::milliseconds m_total_duration;
};

} // namespace Sol2D
