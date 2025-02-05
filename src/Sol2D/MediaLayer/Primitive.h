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

#include <Sol2D/MediaLayer/RenderingContext.h>
#include <Sol2D/MediaLayer/RectRenderer.h>
#include <Sol2D/MediaLayer/LineRenderer.h>
#include <Sol2D/MediaLayer/RenderingData.h>
#include <Sol2D/Def.h>

namespace Sol2D {

class Primitive
{
    S2_DISABLE_COPY_AND_MOVE(Primitive)

public:
    Primitive() {}
    virtual ~Primitive() {}
    virtual void render(const RenderingContext & _context) = 0;
};

class RectPrimitive : public Primitive
{
public:
    RectPrimitive(const RectRenderer & _renderer, const RectRenderingData & data) :
        mr_renderer(_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderRect(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    const RectRenderingData m_data;
};

class SolidRectPrimitive : public Primitive
{
public:
    SolidRectPrimitive(const RectRenderer & _renderer, const SolidRectRenderingData & data) :
        mr_renderer(_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderRect(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    const SolidRectRenderingData m_data;
};

class TexturePrimitive : public Primitive
{
public:
    TexturePrimitive(const RectRenderer & _renderer, const TextureRenderingData & data) :
        mr_renderer(_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderTexture(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    const TextureRenderingData m_data;
};

class LinePrimitive : public Primitive
{
public:
    LinePrimitive(const LineRenderer & _renderer, const LineRenderer::ChunkID & _id, const SDL_FColor & _color) :
        mr_renderer(_renderer),
        m_color(_color),
        m_id(_id)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.render(_context, m_id, m_color);
    }

private:
    const LineRenderer & mr_renderer;
    const SDL_FColor m_color;
    const LineRenderer::ChunkID m_id;
};

class CirclePrimitive : public Primitive
{
public:
    CirclePrimitive(const RectRenderer & _renderer, const CircleRenderingData & data) :
        mr_renderer(_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderCircle(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    const CircleRenderingData m_data;
};

class SolidCirclePrimitive : public Primitive
{
public:
    SolidCirclePrimitive(const RectRenderer & _renderer, const SolidCircleRenderingData & data) :
        mr_renderer(_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderCircle(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    const SolidCircleRenderingData m_data;
};

} // namespace Sol2D
