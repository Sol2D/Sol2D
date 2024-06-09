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

#include <SDL3_image/SDL_image.h>
#include <Sol2D/Sprite.h>
#include <Sol2D/Utils/Math.h>

using namespace Sol2D;
using namespace Sol2D::SDL;
using namespace Sol2D::Utils;

bool Sprite::loadFromFile(const std::filesystem::path & _path, const SpriteOptions & _options /*= SpriteOptions()*/)
{
    m_texture_ptr.reset();
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_options.color_to_alpha.has_value())
    {
        SDL_Color color = _options.color_to_alpha.value();
        SDL_SetSurfaceColorKey(
            surface,
            SDL_TRUE,
            SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a)
        );
    }
    m_source_rect = _options.rect.has_value() ? _options.rect.value() : SDL_FRect {
         .x = .0f,
         .y = .0f,
         .w = static_cast<float>(surface->w),
         .h = static_cast<float>(surface->h)
    };
    m_texture_ptr = wrapSdlTexturePtr(SDL_CreateTextureFromSurface(mp_renderer, surface));
    SDL_DestroySurface(surface);
    return true;
}

void Sprite::render(const SDL_FPoint & _point, SpriteRenderOptions _options /*= SpriteRenderOptions()*/)
{
    if(!isValid())
        return;
    SDL_FRect dest_rect;
    dest_rect.x = _point.x;
    dest_rect.y = _point.y;
    if(_options.size.has_value())
    {
        dest_rect.w = _options.size->x;
        dest_rect.h = _options.size->y;
    }
    else
    {
        dest_rect.w = m_source_rect.w;
        dest_rect.h = m_source_rect.h;
    }
    SDL_RenderTextureRotated(
        mp_renderer,
        m_texture_ptr.get(),
        &m_source_rect,
        &dest_rect,
        radiansToDegrees(_options.angle_rad),
        _options.flip_center.has_value() ? &_options.flip_center.value() : nullptr,
        _options.flip
    );
}
