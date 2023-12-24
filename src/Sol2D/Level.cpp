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

#include <Sol2D/Level.h>
#include <Sol2D/Box2D.h>
#include <Sol2D/Tiles/Tmx.h>

using namespace Sol2D;
using namespace Sol2D::Tiles;

Level::Level(SDL_Renderer & _renderer, const Workspace & _workspace) :
    mp_renderer(&_renderer),
    mr_workspace(_workspace),
    m_world_offset{ .0f, .0f },
    mp_followed_body(nullptr),
    mp_box2d(new Box2D({ .0f, .0f }, 20.0f)) // TODO: from parameters
{
}

Level::~Level()
{
    deinitialize();
    delete mp_box2d;
}

void Level::deinitialize()
{
    for(Body * body : m_bodies)
        delete body;
    m_bodies.clear();
    mp_box2d->reset();
}

bool Level::loadFromTmx(const std::filesystem::path & _tmx_file)
{
    deinitialize();
    m_tile_heap_ptr.reset();
    m_tile_map_ptr.reset();
    m_object_heap_ptr.reset();
    Tmx tmx = loadTmx(*mp_renderer, mr_workspace, _tmx_file); // TODO: handle exceptions
    m_tile_heap_ptr = std::move(tmx.tile_heap);
    m_tile_map_ptr = std::move(tmx.tile_map);
    m_object_heap_ptr = std::move(tmx.object_heap);
    return m_tile_map_ptr != nullptr; // TODO: only exceptions
}

void Level::render(const SDL_FRect & _viewport)
{
    if(!m_tile_map_ptr) return;

    mp_box2d->step(1.0f / 60.0f, 8, 3); // TODO: from user settings

    syncWorldWithFollowedBody(_viewport);
    // TODO: cache texture https://discourse.libsdl.org/t/cost-of-creating-textures-vs-holding-onto-them/38339/2
    SDL_Texture * texture = SDL_CreateTexture(
        mp_renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_TARGET,
        _viewport.w,
        _viewport.h
    );
    SDL_SetRenderTarget(mp_renderer, texture);
    const SDL_Color & bg_color = m_tile_map_ptr->getBackgroundColor();
    SDL_SetRenderDrawColor(mp_renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
    SDL_RenderClear(mp_renderer);
    std::vector<bool> rendered_bodies(m_bodies.size(), false);
    drawLayersAndBodies(*m_tile_map_ptr, _viewport, rendered_bodies);
    for(size_t i = 0; i < m_bodies.size(); ++i)
    {
        if(!rendered_bodies[i])
            drawBody(*m_bodies[i]);
    }
    drawBox2D();
    SDL_SetRenderTarget(mp_renderer, nullptr);
    SDL_FRect src_rect { .0f, .0f, _viewport.w, _viewport.h };
    SDL_RenderTexture(mp_renderer, texture, &src_rect, &_viewport);
    SDL_DestroyTexture(texture);
}

void Level::move(int /*_delta_x*/, int /*_delta_y*/)
{
    // TODO: map moving
    throw NotSupportedException("Map moving is not implemented yet");
    // m_world_offset.x += _delta_x;
    // if(m_world_offset.x < 0)
    //     m_world_offset.x = 0;
    // m_world_offset.y += _delta_y;
    // if(m_world_offset.y < 0)
    //     m_world_offset.y = 0;
}

void Level::drawLayersAndBodies(const TileMapLayerContainer & _container,
                                          const SDL_FRect & _viewport,
                                          std::vector<bool> & _rendered_bodies)
{
    _container.forEachLayer([&_viewport, &_rendered_bodies, this](const TileMapLayer & __layer) {
        if(!__layer.isVisible()) return;
        switch(__layer.getType())
        {
        case TileMapLayerType::tile:
            drawTileLayer(_viewport, dynamic_cast<const TileMapTileLayer &>(__layer));
            break;
        case TileMapLayerType::object:
            drawObjectLayer(dynamic_cast<const TileMapObjectLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::image:
            drawImageLayer(dynamic_cast<const TileMapImageLayer &>(__layer)); // TODO: _viewport
            break;
        case TileMapLayerType::group:
        {
            const TileMapGroupLayer & group = dynamic_cast<const TileMapGroupLayer &>(__layer);
            if(group.isVisible())
                drawLayersAndBodies(group, _viewport, _rendered_bodies);
            break;
        }}
        for(size_t i = 0; i < m_bodies.size(); ++i) // TODO: optimize it out
        {
            Body * body = m_bodies[i];
            if(!_rendered_bodies[i] && body->getLayer() == __layer.getName())
            {
                drawBody(*body);
                _rendered_bodies[i] = true;
            }
        }
    });
}

void Level::drawObjectLayer(const TileMapObjectLayer & _layer)
{
    SDL_SetRenderDrawColor(mp_renderer, 10, 0, 200, 100);
    _layer.forEachObject([this](const TileMapObject & __object) {
        if(!__object.isVisible()) return;
        // TODO: if in viewport
        switch (__object.getObjectType())
        {
        case TileMapObjectType::polygon:
            drawPolyXObject(dynamic_cast<const TileMapPolygon &>(__object), true);
            break;
        case TileMapObjectType::polyline:
            drawPolyXObject(dynamic_cast<const TileMapPolyline &>(__object), false);
            break;
        case TileMapObjectType::circle:
            drawCircle(dynamic_cast<const TileMapCircle &>(__object));
            break;
        default:
            // TODO: point
            break;
        }
    });
}

void Level::drawPolyXObject(const TileMapPolyX & _poly, bool _close)
{
    const std::vector<SDL_FPoint> & poly_points = _poly.getPoints();
    size_t poly_points_count = poly_points.size();
    if(poly_points_count < 2) return;
    size_t total_points_count = _close ? poly_points_count + 1 : poly_points_count;
    SDL_FPoint base_point = toAbsoluteCoords(_poly.getX(), _poly.getY());
    std::vector<SDL_FPoint> points(total_points_count);
    for(size_t i = 0; i < poly_points_count; ++i)
    {
        points[i].x = base_point.x + poly_points[i].x;
        points[i].y = base_point.y + poly_points[i].y;
    }
    if(_close)
    {
        points[poly_points_count].x = points[0].x;
        points[poly_points_count].y = points[0].y;
    }
    SDL_RenderLines(mp_renderer, points.data(), total_points_count);
}

void Level::drawCircle(const TileMapCircle & _circle)
{
    SDL_FPoint position = toAbsoluteCoords(_circle.getX(), _circle.getY());
    sdlRenderCircle(mp_renderer, position, _circle.getRadius());
}

void Level::drawTileLayer(const SDL_FRect & _viewport, const TileMapTileLayer & _layer)
{
    SDL_FRect layer_rect {
        .x = m_world_offset.x,
        .y = m_world_offset.y,
        .w = _viewport.w,
        .h = _viewport.h
    };
    float dest_x = .0f;
    float dest_y = .0f;
    const uint32_t tile_width = m_tile_map_ptr->getTileWidth();
    const uint32_t tile_height = m_tile_map_ptr->getTileHeight();
    if(tile_width == 0 || tile_height == 0)
        return;

    const int32_t layer_x = _layer.getX() * tile_width;
    uint32_t delta_x_abs = std::abs(layer_rect.x - layer_x);
    uint32_t skip_tiles_x = delta_x_abs / tile_width;
    uint32_t first_tile_x_remainder = delta_x_abs % tile_width;
    if(layer_rect.x < layer_x)
    {
        dest_x = skip_tiles_x * tile_width + first_tile_x_remainder;
        skip_tiles_x = 0;
    }
    int32_t first_tile_x = _layer.getX() + skip_tiles_x;
    int32_t max_columns = layer_rect.w / tile_width + 2;

    const int32_t layer_y = _layer.getY() * tile_height;
    uint32_t delta_y_abs = std::abs(layer_rect.y - layer_y);
    uint32_t skip_tiles_y = delta_y_abs / tile_height;
    uint32_t first_tile_y_remainder = delta_y_abs % tile_height;
    if(layer_rect.y < layer_y)
    {
        dest_y = skip_tiles_y * tile_height + first_tile_y_remainder;
        skip_tiles_y = 0;
    }
    int32_t first_tile_y = _layer.getY() + skip_tiles_y;
    int32_t max_rows = layer_rect.h / tile_height + 2;

    int32_t last_tile_x = _layer.getX() + _layer.getWidth();
    int32_t last_tile_y = _layer.getY() + _layer.getHeight();

    SDL_FRect src_rect, dest_rect;
    const Tile * tile;

    for(int32_t tile_y = first_tile_y, row = 0; row <= max_rows && tile_y <= last_tile_y; ++tile_y, ++row)
    {
        for(int32_t tile_x = first_tile_x, col = 0; col <= max_columns && tile_x <= last_tile_x; ++tile_x, ++col)
        {
            tile = _layer.getTile(tile_x, tile_y);
            if(!tile) continue;

            src_rect.x = tile->getSourceX();
            src_rect.y = tile->getSourceY();
            src_rect.w = tile->getWidth();
            src_rect.h = tile->getHeight();

            if(col == 0 && first_tile_x_remainder)
            {
                src_rect.x += first_tile_x_remainder;
                src_rect.w -= first_tile_x_remainder;
            }
            if(row == 0 && first_tile_y_remainder)
            {
                src_rect.y += first_tile_y_remainder;
                src_rect.h -= first_tile_y_remainder;
            }

            dest_rect.x = (dest_x + col) * tile_width;
            dest_rect.y = (dest_y + row) * tile_height;
            dest_rect.w = src_rect.w;
            dest_rect.h = src_rect.h;

            if(col != 0) dest_rect.x -= first_tile_x_remainder;
            if(row != 0) dest_rect.y -= first_tile_y_remainder;

            SDL_RenderTexture(mp_renderer, &tile->getSource(), &src_rect, &dest_rect);
        }
    }
}

void Level::drawImageLayer(const TileMapImageLayer & _layer)
{
    SDL_TexturePtr image = _layer.getImage();
    int width, height;
    SDL_QueryTexture(image.get(), NULL, NULL, &width, &height);
    SDL_FRect dim { .0f, .0f, static_cast<float>(width), static_cast<float>(height) };
    SDL_RenderTexture(mp_renderer, image.get(), nullptr, &dim);
}

void Level::drawBody(const Body & _body) const
{
    // TODO: if visible
    // TODO: if in viewport
    const Sprite * sprite = _body.getActiveSprite();
    if(!sprite) return;
    SDL_FPoint body_position = _body.getPosition();
    body_position = toAbsoluteCoords(body_position.x, body_position.y);
    SDL_FRect dest_rect {
        body_position.x,
        body_position.y,
        static_cast<float>(sprite->rect.w),
        static_cast<float>(sprite->rect.h)
    };
    SDL_FRect sprite_rect {
        .x = static_cast<float>(sprite->rect.x),
        .y = static_cast<float>(sprite->rect.y),
        .w = static_cast<float>(sprite->rect.w),
        .h = static_cast<float>(sprite->rect.h)
    };
    SDL_RenderTextureRotated(mp_renderer, sprite->texture, &sprite_rect, &dest_rect, 0.0, nullptr, _body.getFlip());
}

void Level::drawBox2D()
{
    const float scale_factor = mp_box2d->getScaleFactor();
    for(b2Body * body = mp_box2d->getBodyList(); body; body = body->GetNext())
    {
        const b2Vec2 & body_pos = body->GetPosition();
        for(b2Fixture * fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext())
        {
            if(fixture->IsSensor())
                SDL_SetRenderDrawColor(mp_renderer, 20, 255, 20, 100);
            else
                SDL_SetRenderDrawColor(mp_renderer, 255, 20, 20, 100);
            b2Shape * shape = fixture->GetShape();
            switch(shape->GetType()) {
            case b2Shape::e_polygon:
            {
                b2PolygonShape * polygon = dynamic_cast<b2PolygonShape *>(shape);
                for(int32 i = 1; i < polygon->m_count; ++i)
                {
                    SDL_FPoint a = toAbsoluteCoords(
                        (polygon->m_vertices[i - 1].x + body_pos.x) * scale_factor,
                        (polygon->m_vertices[i - 1].y + body_pos.y) * scale_factor
                    );
                    SDL_FPoint b = toAbsoluteCoords(
                        (polygon->m_vertices[i].x + body_pos.x) * scale_factor,
                        (polygon->m_vertices[i].y + body_pos.y) * scale_factor
                    );
                    SDL_RenderLine(mp_renderer, a.x, a.y, b.x, b.y);
                }
                SDL_FPoint a = toAbsoluteCoords(
                    (polygon->m_vertices[0].x + body_pos.x) * scale_factor,
                    (polygon->m_vertices[0].y + body_pos.y) * scale_factor
                );
                SDL_FPoint b = toAbsoluteCoords(
                    (polygon->m_vertices[polygon->m_count - 1].x + body_pos.x) * scale_factor,
                    (polygon->m_vertices[polygon->m_count - 1].y + body_pos.y) * scale_factor
                );
                SDL_RenderLine(mp_renderer, a.x, a.y, b.x, b.y);
                break;
            }
            case b2Shape::e_circle:
            {
                b2CircleShape * circle = dynamic_cast<b2CircleShape *>(shape);
                SDL_FPoint center = toAbsoluteCoords(
                    body_pos.x * scale_factor,
                    body_pos.y * scale_factor
                );
                sdlRenderCircle(mp_renderer, center, static_cast<uint32_t>(circle->m_radius * scale_factor));
                break;
            }
            default:
                break;
            }
        }
    }
}

void Level::syncWorldWithFollowedBody(const SDL_FRect & _viewport)
{
   if(mp_followed_body == nullptr)
       return;
   const SDL_FPoint & body_position = mp_followed_body->getPosition();
   m_world_offset.x = body_position.x - static_cast<float>(_viewport.w) / 2;
   m_world_offset.y = body_position.y - static_cast<float>(_viewport.h) / 2;
   const int32_t map_x = m_tile_map_ptr->getX() * m_tile_map_ptr->getTileWidth();
   const int32_t map_y = m_tile_map_ptr->getY() * m_tile_map_ptr->getTileHeight();
   if(m_world_offset.x < map_x)
   {
       m_world_offset.x = map_x;
   }
   else
   {
       int32_t max_offset_x = m_tile_map_ptr->getWidth() * m_tile_map_ptr->getTileWidth() - _viewport.w;
       if(m_world_offset.x > max_offset_x)
           m_world_offset.x = max_offset_x;
   }
   if(m_world_offset.y < map_y)
   {
       m_world_offset.y = map_y;
   }
   else
   {
       int32_t max_offset_y = m_tile_map_ptr->getHeight() * m_tile_map_ptr->getTileHeight() - _viewport.h;
       if(m_world_offset.y > max_offset_y)
           m_world_offset.y = max_offset_y;
   }
}

SDL_FPoint Level::toAbsoluteCoords(float _world_x, float _world_y) const
{
    SDL_FPoint result;
    result.x = _world_x - m_world_offset.x;
    result.y = _world_y - m_world_offset.y;
    return result;
}

Body & Level::createBody(const BodyPrototype & _proto)
{
    Body * body = new Body(_proto, *mp_box2d);
    m_bodies.push_back(body);
    return *body;
}

void Level::createBox2dStaticBodies(const std::string _class, Box2D::StaticObjectKind _kind)
{
    m_object_heap_ptr->forEachObject(_class, [this, _kind](TileMapObject & __object) {
        // TODO: what if it is already created?
        mp_box2d->createStaticBody(__object, _kind);
    });
}
