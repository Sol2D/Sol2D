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
    RectPrimitive(const RectRenderer & mr_renderer, const RectRenderingData & data) :
        mr_renderer(mr_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderRect(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    RectRenderingData m_data;
};

class SolidRectPrimitive : public Primitive
{
public:
    SolidRectPrimitive(const RectRenderer & mr_renderer, const SolidRectRenderingData & data) :
        mr_renderer(mr_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderRect(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    SolidRectRenderingData m_data;
};

class TexturePrimitive : public Primitive
{
public:
    TexturePrimitive(const RectRenderer & mr_renderer, const TextureRenderingData & data) :
        mr_renderer(mr_renderer),
        m_data(data)
    {
    }

    void render(const RenderingContext & _context) override
    {
        mr_renderer.renderTexture(_context, m_data);
    }

private:
    const RectRenderer & mr_renderer;
    TextureRenderingData m_data;
};


} // namespace Sol2D
