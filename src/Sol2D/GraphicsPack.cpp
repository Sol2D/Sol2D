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

#include <Sol2D/GraphicsPack.h>
#include <Sol2D/Utils/Math.h>

using namespace Sol2D;
using namespace Sol2D::Utils;

struct GraphicsPack::Graphics
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Graphics)
    explicit Graphics(const GraphicsPackSpriteDefinition & _definition);

public:
    std::optional<Sprite> sprite;
    bool is_visible;
    Point position;

private:
    static std::optional<Sprite> getSprite(const GraphicsPackSpriteDefinition & _definition);
};

inline GraphicsPack::Graphics::Graphics(const GraphicsPackSpriteDefinition & _definition) :
    sprite(getSprite(_definition)),
    is_visible(_definition.is_visible),
    position(_definition.position)
{
}

std::optional<Sprite> GraphicsPack::Graphics::getSprite(const GraphicsPackSpriteDefinition & _definition)
{
    if(std::holds_alternative<std::shared_ptr<Sprite>>(_definition.sprite))
    {
        std::shared_ptr<Sprite> sprite = std::get<std::shared_ptr<Sprite>>(_definition.sprite);
        return sprite ? std::make_optional(*sprite) : std::nullopt;
    }
    else if(std::holds_alternative<GraphicsPackSpriteSheetSpriteDefinition>(_definition.sprite))
    {
        const GraphicsPackSpriteSheetSpriteDefinition & sheet_def =
            std::get<GraphicsPackSpriteSheetSpriteDefinition>(_definition.sprite);
        return sheet_def.sprite_sheet->getSpriteCount() > sheet_def.sprite_index
            ? std::make_optional(sheet_def.sprite_sheet->toSprite(sheet_def.sprite_index))
            : std::nullopt;
    }
    else
    {
        throw std::runtime_error("Unknown sprite source");
    }
}

struct GraphicsPack::Frame
{
    S2_DEFAULT_COPY_AND_MOVE(Frame)
    explicit Frame(const GraphicsPackFrameDefinition & _definition);
    std::chrono::milliseconds duration;
    std::vector<Graphics> graphics;
    bool is_visible;
};

inline GraphicsPack::Frame::Frame(const GraphicsPackFrameDefinition & _definition) :
    duration(_definition.duration),
    is_visible(_definition.is_visible)
{
    graphics.reserve(_definition.sprites.size());
    for(const auto & sprite_def : _definition.sprites)
        graphics.emplace_back(sprite_def); // cppcheck-suppress useStlAlgorithm
}

GraphicsPack::GraphicsPack(
    SDL_Renderer & _renderer,
    const GraphicsPackDefinition & _definition /*= GraphicsPackDefinition()*/
) :
    mp_renderer(&_renderer),
    m_position(_definition.position),
    m_flip_mode(SDL_FLIP_NONE),
    m_flip_center(_definition.flip_center),
    m_max_iterations(_definition.animation_iterations),
    m_current_iteration(0),
    m_current_frame_index(0),
    m_current_frame_duration(std::chrono::milliseconds::zero()),
    m_total_duration(std::chrono::milliseconds::zero())
{
    if(_definition.is_flipped_horizontally)
        m_flip_mode = static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) | SDL_FLIP_HORIZONTAL);
    if(_definition.is_flipped_vertically)
        m_flip_mode = static_cast<SDL_FlipMode>(static_cast<int>(m_flip_mode) | SDL_FLIP_VERTICAL);
    for(const auto & frame_def : _definition.frames)
    {
        addFrame(frame_def);
    }
}

GraphicsPack::GraphicsPack(const GraphicsPack & _graphics_pack) :
    mp_renderer(_graphics_pack.mp_renderer),
    m_position(_graphics_pack.m_position),
    m_flip_mode(_graphics_pack.m_flip_mode),
    m_max_iterations(_graphics_pack.m_max_iterations),
    m_current_iteration(0),
    m_current_frame_index(_graphics_pack.m_current_frame_index),
    m_current_frame_duration(_graphics_pack.m_current_frame_duration),
    m_total_duration(_graphics_pack.m_total_duration)
{
    m_frames.reserve(_graphics_pack.m_frames.size());
    for(auto * frame : _graphics_pack.m_frames)
        m_frames.push_back(new Frame(*frame)); // cppcheck-suppress useStlAlgorithm
}

GraphicsPack::GraphicsPack(GraphicsPack && _graphics_pack) :
    mp_renderer(_graphics_pack.mp_renderer),
    m_position(_graphics_pack.m_position),
    m_flip_mode(_graphics_pack.m_flip_mode),
    m_frames(std::move(_graphics_pack.m_frames)),
    m_max_iterations(_graphics_pack.m_max_iterations),
    m_current_iteration(0),
    m_current_frame_index(_graphics_pack.m_current_frame_index),
    m_current_frame_duration(_graphics_pack.m_current_frame_duration),
    m_total_duration(_graphics_pack.m_total_duration)
{
    _graphics_pack.mp_renderer = nullptr;
    _graphics_pack.m_current_frame_duration = std::chrono::milliseconds::zero();
    _graphics_pack.m_total_duration = std::chrono::milliseconds::zero();
    _graphics_pack.m_current_frame_index = 0;
}

GraphicsPack::~GraphicsPack()
{
    destroy();
}

GraphicsPack & GraphicsPack::operator = (const GraphicsPack & _graphics_pack)
{
    if(this != &_graphics_pack)
    {
        destroy();
        m_frames.reserve(_graphics_pack.m_frames.size());
        for(auto * frame : _graphics_pack.m_frames)
            m_frames.push_back(new Frame(*frame)); // cppcheck-suppress useStlAlgorithm
        mp_renderer = _graphics_pack.mp_renderer;
        m_flip_mode = _graphics_pack.m_flip_mode;
        m_position = _graphics_pack.m_position;
        m_max_iterations = _graphics_pack.m_max_iterations;
        m_current_iteration = 0;
        m_current_frame_index = _graphics_pack.m_current_frame_index;
        m_current_frame_duration = _graphics_pack.m_current_frame_duration;
        m_total_duration = _graphics_pack.m_total_duration;
    }
    return *this;
}

GraphicsPack & GraphicsPack::operator = (GraphicsPack && _graphics_pack)
{
    if(this != &_graphics_pack)
    {
        destroy();
        m_frames.reserve(_graphics_pack.m_frames.size());
        std::copy(_graphics_pack.m_frames.cbegin(), _graphics_pack.m_frames.cend(), m_frames.end());
        _graphics_pack.m_frames.clear();
        mp_renderer = _graphics_pack.mp_renderer;
        _graphics_pack.mp_renderer = nullptr;
        m_position = _graphics_pack.m_position;
        m_flip_mode = _graphics_pack.m_flip_mode;
        m_max_iterations = _graphics_pack.m_max_iterations;
        m_current_iteration = 0;
        m_current_frame_index = _graphics_pack.m_current_frame_index;
        _graphics_pack.m_current_frame_index = 0;
        m_current_frame_duration = _graphics_pack.m_current_frame_duration;
        _graphics_pack.m_current_frame_duration = std::chrono::milliseconds::zero();
        m_total_duration = _graphics_pack.m_total_duration;
        _graphics_pack.m_total_duration = std::chrono::milliseconds::zero();
    }
    return *this;
}

void GraphicsPack::destroy()
{
    for(Frame * frame : m_frames)
        delete frame;
    m_frames.clear();
}

size_t GraphicsPack::addFrame(const GraphicsPackFrameDefinition & _definition)
{
    Frame * frame = new Frame(_definition);
    m_frames.push_back(frame);
    if(frame->is_visible)
        m_total_duration += frame->duration;
    return m_frames.size() - 1;
}

size_t GraphicsPack::insertFrame(size_t _index, const GraphicsPackFrameDefinition & _definition)
{
    Frame * frame = new Frame(_definition);
    m_frames.insert(m_frames.begin() + _index, frame);
    if(frame->is_visible)
        m_total_duration += frame->duration;
    return _index;
}

bool GraphicsPack::removeFrame(size_t _index)
{
    auto it = m_frames.begin() + _index;
    if(it == m_frames.end())
        return false;
    const Frame * frame = *it;
    if(frame->is_visible)
        m_total_duration -= frame->duration;
    m_frames.erase(it);
    if(m_current_frame_index >= m_frames.size())
    {
        m_current_frame_index = 0;
        m_current_frame_duration = std::chrono::milliseconds::zero();
    }
    return true;
}

bool GraphicsPack::setFrameVisibility(size_t _index, bool _is_visible)
{
    if(_index < m_frames.size())
    {
        Frame * frame = m_frames[_index];
        if(frame->is_visible && !_is_visible)
        {
            m_total_duration -= frame->duration;
            frame->is_visible = false;
        }
        else if(!frame->is_visible && _is_visible)
        {
            m_total_duration += frame->duration;
            frame->is_visible = true;
        }
        return true;
    }
    return false;
}

std::optional<bool> GraphicsPack::isFrameVisible(size_t _index) const
{
    if(_index < m_frames.size())
        return m_frames[_index]->is_visible;
    return std::nullopt;
}

bool GraphicsPack::setFrameDuration(size_t _index, std::chrono::milliseconds _duration)
{
    if(_index < m_frames.size())
    {
        Frame * frame = m_frames[_index];
        if(frame->is_visible)
            m_total_duration = m_total_duration - frame->duration + _duration;
        frame->duration = _duration;
        return true;
    }
    return false;
}

std::optional<std::chrono::milliseconds> GraphicsPack::getFrameDuration(size_t _index) const
{
    if(_index < m_frames.size())
    {
        return m_frames[_index]->duration;
    }
    return std::nullopt;
}

bool GraphicsPack::setCurrentFrameIndex(size_t _index)
{
    if(_index >= m_frames.size() || !m_frames[_index]->is_visible)
        return false;
    m_current_frame_index = _index;
    m_current_frame_duration = std::chrono::milliseconds::zero();
    return true;
}

std::pair<bool, size_t> GraphicsPack::addSprite(size_t _frame, const GraphicsPackSpriteDefinition & _definition)
{
    if(_frame >= m_frames.size())
        return std::make_pair(false, 0);
    std::vector<Graphics> & graphics = m_frames[_frame]->graphics;
    graphics.emplace_back(_definition);
    return std::make_pair(true, graphics.size() - 1);
}

bool GraphicsPack::removeSprite(size_t _frame, size_t _sprite)
{
    if(_frame >= m_frames.size())
        return false;
    std::vector<Graphics> & graphics = m_frames[_frame]->graphics;
    if(_sprite >= graphics.size())
        return false;
    graphics.erase(graphics.begin() + _sprite);
    return true;
}

void GraphicsPack::render(const Point & _position, float _angle_deg, std::chrono::milliseconds _time_passed)
{
    if(m_max_iterations == 0 || m_total_duration == std::chrono::milliseconds::zero())
    {
        performRender(_position, _angle_deg);
        return;
    }
    m_current_frame_duration += _time_passed;
    const Frame * frame = m_frames[m_current_frame_index];
    bool respect_iterations = m_max_iterations > 0;
    if(!frame->is_visible)
    {
        if(switchToNextVisibleFrame(respect_iterations))
        {
            m_current_frame_duration = _time_passed;
        }
        else
        {
            m_current_frame_duration = std::chrono::milliseconds::zero();
            return;
        }
    }
    for(;;)
    {
        if(frame->duration < m_current_frame_duration)
        {
            m_current_frame_duration -= frame->duration;
            if(!switchToNextVisibleFrame(respect_iterations))
                break;
            frame = m_frames[m_current_frame_index];
        }
        else
        {
            break;
        }
    }
    performRender(_position, _angle_deg);
}

bool GraphicsPack::switchToNextVisibleFrame()
{
    return switchToNextVisibleFrame(false);
}

bool GraphicsPack::switchToNextVisibleFrame(bool _respect_iteration)
{
    if(_respect_iteration && m_current_iteration > m_max_iterations)
        return false;

    for(size_t i = m_current_frame_index + 1; i < m_frames.size(); ++i)
    {
        if(m_frames[i]->is_visible)
        {
            m_current_frame_index = i;
            return true;
        }
    }

    if(m_current_frame_index > 0)
    {
        if(_respect_iteration && ++m_current_iteration > m_max_iterations)
            return false;

        for(size_t i = 0; i < m_current_frame_index; ++i)
        {
            if(m_frames[i]->is_visible)
            {
                m_current_frame_index = i;
                return true;
            }
        }
    }

    return m_frames[m_current_frame_index]->is_visible;
}

void GraphicsPack::performRender(const Point & _position, float _angle_deg)
{
    if(m_frames.empty())
        return;
    Frame * frame = m_frames[m_current_frame_index];
    if(!frame->is_visible)
        return;
    Point position = _position + m_position;
    for(Graphics & graphics : frame->graphics)
    {
        if(graphics.sprite.has_value() && graphics.is_visible)
        {
            graphics.sprite->render(
                position + graphics.position,
                _angle_deg,
                m_flip_mode,
                m_flip_center.has_value() ? &m_flip_center.value() : nullptr);
        }
    }
}
