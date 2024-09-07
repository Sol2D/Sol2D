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
#include <optional>
#include <chrono>
#include <vector>

namespace Sol2D {

struct GraphicsPackFrameOptions
{
    std::optional<std::chrono::milliseconds> duration;
    std::optional<bool> is_visible;
};

struct GraphicsPackSpriteOptions
{
    std::optional<Point> position;
    std::optional<float> angle_rad;
    std::optional<std::optional<Point>> flip_center;
    std::optional<bool> is_flipped_horizontally;
    std::optional<bool> is_flipped_vertically;
    std::optional<bool> is_visible;
};

class GraphicsPack final
{
    struct Graphics
    {
        S2_DEFAULT_COPY_AND_MOVE(Graphics)

        Graphics(const Sprite & _sprite, const GraphicsPackSpriteOptions & _options) :
            Graphics(_sprite.getTexture(), _sprite.getSourceRect(), _sprite.getDestinationSize(), _options)
        {
        }

        Graphics(
            std::shared_ptr<SDL_Texture> _texture,
            const Rect & _src_rect,
            const Size & _dest_size,
            const GraphicsPackSpriteOptions & _options
        ) :
            texture(_texture),
            src_rect(_src_rect),
            dest_size(_dest_size),
            position(_options.position.value_or(Point {.0f, .0f})),
            angle_rad(_options.angle_rad.value_or(.0f)),
            flip_center(_options.flip_center.value_or(Point { .0f, .0f })),
            is_flipped_horizontally(_options.is_flipped_horizontally.value_or(false)),
            is_flipped_vertically(_options.is_flipped_vertically.value_or(false)),
            is_visible(_options.is_visible.value_or(true))
        {
        }

        std::shared_ptr<SDL_Texture> texture;
        Rect src_rect;
        Size dest_size;
        Point position;
        float angle_rad;
        std::optional<Point> flip_center;
        bool is_flipped_horizontally;
        bool is_flipped_vertically;
        bool is_visible;
        std::shared_ptr<Sprite> sprite;
    };

    struct Frame
    {
        S2_DEFAULT_COPY_AND_MOVE(Frame)

        Frame(const GraphicsPackFrameOptions & _options) :
            duration(_options.duration.value_or(std::chrono::milliseconds::zero())),
            is_visible(_options.is_visible.value_or(true))
        {
        }

        std::chrono::milliseconds duration;
        bool is_visible;
        std::vector<Graphics> graphics;
    };

public:
    S2_DEFAULT_COPY_AND_MOVE(GraphicsPack)

    explicit GraphicsPack(SDL_Renderer & _renderer);
    ~GraphicsPack();

    size_t addFrame(const GraphicsPackFrameOptions & _options);
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
    void render(const Point & _point, std::chrono::milliseconds _time_passed);

private:
    void performRender(const Point & _point);
    Frame * switchToNextVisibleFrame();

private:
    SDL_Renderer * mp_renderer;
    std::vector<Frame *> m_frames;
    size_t m_current_frame_index;
    std::chrono::milliseconds m_current_frame_duration;
    std::chrono::milliseconds m_total_duration;
};

} // namespace Sol2D
