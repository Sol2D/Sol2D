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

#include <Sol2D/SpriteAnimation.h>
#include <Sol2D/Utils/Math.h>

using namespace Sol2D;
using namespace Sol2D::Utils;

namespace {

inline std::chrono::milliseconds getDuration(std::optional<std::chrono::milliseconds> _opt_duration)
{
    if(!_opt_duration.has_value())
        return std::chrono::milliseconds::zero();
    return _opt_duration.value() < std::chrono::milliseconds::zero()
        ? std::chrono::milliseconds::zero()
        : _opt_duration.value();
}

inline Point getPosition(const std::optional<Point> & _point)
{
    return _point.value_or(makePoint(.0f, .0f));
}

} // namespace

struct SpriteAnimation::Frame
{
    std::shared_ptr<SDL_Texture> texture;
    Rect src_rect;
    Size dest_size;
    std::chrono::milliseconds duration;
    Point position;
};

SpriteAnimation::SpriteAnimation(SDL_Renderer & _renderer) :
    mp_renderer(&_renderer),
    m_current_frame_index(0),
    m_current_frame_duration(0),
    m_total_duration(0)
{
}

SpriteAnimation::SpriteAnimation(const SpriteAnimation & _animation)
{
    copy(_animation, *this);
}

SpriteAnimation::SpriteAnimation(SpriteAnimation && _animation) :
    mp_renderer(_animation.mp_renderer),
    m_frames(std::move(_animation.m_frames)),
    m_current_frame_index(_animation.m_current_frame_index),
    m_current_frame_duration(_animation.m_current_frame_duration),
    m_total_duration(_animation.m_total_duration)
{
    _animation.m_current_frame_duration = std::chrono::milliseconds::zero();
    _animation.m_current_frame_index = 0;
}

SpriteAnimation::~SpriteAnimation()
{
    clear();
}

SpriteAnimation & SpriteAnimation::operator = (const SpriteAnimation & _animation)
{
    if(this != &_animation)
        copy(_animation, *this);
    return *this;
}

SpriteAnimation & SpriteAnimation::operator = (SpriteAnimation && _animation)
{
    if(this != &_animation)
    {
        clear();
        m_frames = std::move(_animation.m_frames);
        m_current_frame_index = _animation.m_current_frame_index;
        m_current_frame_duration = _animation.m_current_frame_duration;
        m_total_duration = _animation.m_total_duration;
        _animation.m_current_frame_duration = std::chrono::milliseconds::zero();
        _animation.m_current_frame_index = 0;
    }
    return *this;
}

void SpriteAnimation::clear()
{
    for(Frame * frame : m_frames)
        delete frame;
    m_frames.clear();
    m_current_frame_duration = std::chrono::milliseconds::zero();
    m_current_frame_index = 0;
}

void SpriteAnimation::copy(const SpriteAnimation & _from, SpriteAnimation & _to)
{
    _to.clear();
    _to.mp_renderer = _from.mp_renderer;
    _to.m_frames.reserve(_from.m_frames.size());
    _to.m_total_duration = _from.m_total_duration;
    for(Frame * frame : _from.m_frames)
        _to.m_frames.push_back(new Frame(*frame));
}

bool SpriteAnimation::addFrame(const Sprite & _sprite, const SpriteAnimationOptions & _options)
{
    if(!_sprite.isValid())
        return false;
    Frame * frame = new Frame
    {
        .texture = _sprite.getTexture(),
        .src_rect = _sprite.getSourceRect(),
        .dest_size = _sprite.getDestinationSize(),
        .duration = getDuration(_options.duration),
        .position = getPosition(_options.position)
    };
    m_frames.push_back(frame);
    m_total_duration += frame->duration;
    return true;
}

bool SpriteAnimation::addFrame(
    const SpriteSheet & _sprite_sheet,
    size_t _sprite_index,
    const SpriteAnimationOptions & _options)
{
    if(!_sprite_sheet.isValid() || _sprite_sheet.getSpriteCount() <= _sprite_index)
        return false;
    const Rect & rect = _sprite_sheet.getRects().at(_sprite_index);
    Frame * frame = new Frame
    {
        .texture = _sprite_sheet.getTexture(),
        .src_rect = rect,
        .dest_size = rect.getSize(),
        .duration = getDuration(_options.duration),
        .position = getPosition(_options.position)
    };
    m_frames.push_back(frame);
    m_total_duration += frame->duration;
    return true;
}

bool SpriteAnimation::addFrames(
    const SpriteSheet & _sprite_sheet,
    std::vector<size_t> _sprite_indices,
    const SpriteAnimationOptions & _options)
{
    if(!_sprite_sheet.isValid())
        return false;
    size_t sheet_size = _sprite_sheet.getRects().size();
    size_t count = 0;
    for(size_t index : _sprite_indices)
    {
        if(index >= sheet_size)
            return false;
        ++count;
    }
    m_frames.reserve(m_frames.size() + count);
    for(size_t index : _sprite_indices)
    {
        const Rect & rect = _sprite_sheet.getRects().at(index);
        Frame * frame = new Frame
        {
            .texture = _sprite_sheet.getTexture(),
            .src_rect = rect,
            .dest_size = {
                .w = rect.w,
                .h = rect.h
            },
            .duration = getDuration(_options.duration),
            .position = getPosition(_options.position)
        };
        m_frames.push_back(frame);
        m_total_duration += frame->duration;
    }
    return true;
}

void SpriteAnimation::render(
    const Point & _point,
    std::chrono::milliseconds _time_passed,
    SpriteRenderOptions _options /*= SpriteRenderOptions()*/)
{
    if(m_frames.size() == 0)
        return;
    m_current_frame_duration += _time_passed;
    Frame * frame = nullptr;
    if(m_total_duration == std::chrono::milliseconds::zero())
    {
        frame = m_frames[0];
    }
    else
    {
        for(;;)
        {
            frame = m_frames[m_current_frame_index];
            if(frame->duration < m_current_frame_duration)
            {
                m_current_frame_duration -= frame->duration;
                ++m_current_frame_index;
                if(m_current_frame_index >= m_frames.size())
                    m_current_frame_index = 0;
            }
            else
            {
                break;
            }
        }
    }
    SDL_FRect dest_rect
    {
        .x = _point.x - frame->position.x,
        .y = _point.y - frame->position.y,
        .w = frame->dest_size.w,
        .h = frame->dest_size.h
    };
    SDL_RenderTextureRotated(
        mp_renderer,
        frame->texture.get(),
        frame->src_rect.toSdlPtr(),
        &dest_rect,
        radiansToDegrees(_options.angle_rad),
        _options.flip_center.has_value() ? _options.flip_center->toSdlPtr() : nullptr,
        _options.flip
    );
}
