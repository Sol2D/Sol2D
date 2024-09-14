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

#include <Sol2D/Def.h>
#include <Sol2D/Rect.h>
#include <SDL3/SDL_render.h>
#include <box2d/types.h>
#include <functional>

namespace Sol2D {

class Box2dDebugDraw
{
public:
    S2_DEFAULT_COPY_AND_MOVE(Box2dDebugDraw)

    Box2dDebugDraw(
        SDL_Renderer & _renderer,
        b2WorldId _world_id,
        std::function<Point(float, float)> _translate_point,
        std::function<float(float)> _translate_length);
    void draw();

private:
    void setRendererDrawColor(b2HexColor _color);
    static void drawPolygon(const b2Vec2 * _vertices, int _vertex_count, b2HexColor _color, void * _context);
    static void drawSolidPolygon(
        b2Transform _transform,
        const b2Vec2 * _vertices,
        int _vertex_count,
        float _radius,
        b2HexColor _color,
        void * _context);
    static void drawCircle(b2Vec2 _center, float _radius, b2HexColor _color, void * _context);
    static void drawSolidCircle(b2Transform _transform, float _radius, b2HexColor _color, void * _context);
    static void drawPoint(b2Vec2 _point, float _size, b2HexColor _color, void * _context);
    static void drawSegment(b2Vec2 _p1, b2Vec2 _p2, b2HexColor _color, void * _context);

private:
    b2DebugDraw m_b2_debug_draw;
    SDL_Renderer * mp_renderer;
    b2WorldId m_world_id;
    std::function<Point(float, float)> m_translate_point;
    std::function<float(float)> m_translate_length;
};

} // namespace Sol2D
