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

#include <Sol2D/Box2dDebugDraw.h>
#include <Sol2D/SDL/SDL.h>
#include <box2d/box2d.h>

using namespace Sol2D;

Box2dDebugDraw::Box2dDebugDraw(
    SDL_Renderer & _renderer,
    b2WorldId _world_id,
    std::function<Point (float, float)> _translate_point,
    std::function<float(float)> _translate_length
) :
    m_b2_debug_draw(b2DefaultDebugDraw()),
    mp_renderer(&_renderer),
    m_world_id(_world_id),
    m_translate_point(_translate_point),
    m_translate_length(_translate_length)
{
    m_b2_debug_draw.context = this;
    m_b2_debug_draw.drawShapes = true; // TODO: from user config
    m_b2_debug_draw.drawAABBs = false; // TODO: from user config
    m_b2_debug_draw.drawJoints = true; // TODO: from user config
    m_b2_debug_draw.drawContacts = true; // TODO: from user config
    m_b2_debug_draw.DrawPolygon = &Box2dDebugDraw::drawPolygon;
    m_b2_debug_draw.DrawSolidPolygon = &Box2dDebugDraw::drawSolidPolygon;
    m_b2_debug_draw.DrawCircle = &Box2dDebugDraw::drawCircle;
    m_b2_debug_draw.DrawSolidCircle = &Box2dDebugDraw::drawSolidCircle;
    m_b2_debug_draw.DrawPoint = &Box2dDebugDraw::drawPoint;
    m_b2_debug_draw.DrawSegment = &Box2dDebugDraw::drawSegment;
}

void Box2dDebugDraw::draw()
{
    b2World_Draw(m_world_id, &m_b2_debug_draw);
}

void Box2dDebugDraw::drawPolygon(const b2Vec2 * _vertices, int _vertex_count, b2HexColor _color, void * _context)
{
    Box2dDebugDraw * self = static_cast<Box2dDebugDraw *>(_context);
    std::vector<SDL_FPoint> sdl_points;
    sdl_points.reserve(_vertex_count + 1);
    for(int i = 0; i < _vertex_count; ++i)
        sdl_points.push_back(self->m_translate_point(_vertices[i].x, _vertices[i].y));
    sdl_points.push_back(sdl_points[0]);
    self->setRendererDrawColor(_color);
    SDL_RenderLines(self->mp_renderer, sdl_points.data(), sdl_points.size());
}

void Box2dDebugDraw::setRendererDrawColor(b2HexColor _color)
{
    SDL_SetRenderDrawColor(
        mp_renderer,
        (_color & 0xff0000) >> 16,
        (_color & 0x00ff00) >> 8,
        (_color & 0x0000ff),
        255);
}

void Box2dDebugDraw::drawSolidPolygon(
    b2Transform _transform,
    const b2Vec2 * _vertices,
    int _vertex_count,
    float _radius,
    b2HexColor _color,
    void * _context )
{
    S2_UNUSED(_radius)
    Box2dDebugDraw * self = static_cast<Box2dDebugDraw *>(_context);
    std::vector<SDL_FPoint> sdl_points;
    sdl_points.reserve(_vertex_count + 1);
    float x, y;
    for(int i = 0; i < _vertex_count; ++i)
    {
        if(_transform.q.s)
        {
            x = static_cast<float>(_transform.q.c * _vertices[i].x - _transform.q.s * _vertices[i].y) + _transform.p.x;
            y = static_cast<float>(_transform.q.s * _vertices[i].x + _transform.q.c  * _vertices[i].y) + _transform.p.y;
        }
        else
        {
            x = _vertices[i].x + _transform.p.x;
            y = _vertices[i].y + _transform.p.y;
        }
        sdl_points.push_back(self->m_translate_point(x, y));
    }
    sdl_points.push_back(sdl_points[0]);
    self->setRendererDrawColor(_color);
    SDL_RenderLines(self->mp_renderer, sdl_points.data(), sdl_points.size());
}

void Box2dDebugDraw::drawCircle(b2Vec2 _center, float _radius, b2HexColor _color, void * _context)
{
    Box2dDebugDraw * self = static_cast<Box2dDebugDraw *>(_context);
    self->setRendererDrawColor(_color);
    Sol2D::SDL::sdlRenderCircle(
        self->mp_renderer,
        self->m_translate_point(_center.x, _center.y),
        self->m_translate_length(_radius));
}

inline void Box2dDebugDraw::drawSolidCircle(b2Transform _transform, float _radius, b2HexColor _color, void * _context)
{
    drawCircle(_transform.p, _radius, _color, _context);
}

void Box2dDebugDraw::drawPoint(b2Vec2 _point, float _size, b2HexColor _color, void * _context)
{
    Box2dDebugDraw * self = static_cast<Box2dDebugDraw *>(_context);
    self->setRendererDrawColor(_color);
    const Point point = self->m_translate_point(_point.x, _point.y);
    const float half_size = _size / 2;
    const SDL_FRect rect =
    {
        .x = point.x - half_size,
        .y = point.y - half_size,
        .w = _size,
        .h = _size
    };
    SDL_RenderFillRect(self->mp_renderer, &rect);
}

void Box2dDebugDraw::drawSegment(b2Vec2 _p1, b2Vec2 _p2, b2HexColor _color, void * _context)
{
    Box2dDebugDraw * self = static_cast<Box2dDebugDraw *>(_context);
    self->setRendererDrawColor(_color);
    Point points[] =
    {
        self->m_translate_point(_p1.x, _p1.y),
        self->m_translate_point(_p2.x, _p2.y),
    };
    SDL_RenderLine(self->mp_renderer, points[0].x, points[0].y, points[1].x, points[1].y);
}
