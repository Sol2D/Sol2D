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

#include <Sol2D/Sprite.h>
#include <SDL3_image/SDL_image.h>

using namespace Sol2D;

bool Sprite::loadFromFile(const std::filesystem::path & _path, const SpriteOptions & _options /*= SpriteOptions()*/)
{
    SDL_Surface * surface = IMG_Load(_path.c_str());
    if(!surface)
        return false;
    if(_options.color_to_alpha.has_value())
    {
        SDL_Color color = toR8G8B8A8_UINT(_options.color_to_alpha.value());
        const SDL_PixelFormatDetails * pixel_format = SDL_GetPixelFormatDetails(surface->format);
        SDL_SetSurfaceColorKey(surface, true, SDL_MapRGBA(pixel_format, nullptr, color.r, color.g, color.b, color.a));
    }
    if(_options.autodetect_rect)
    {
        SDL_Rect content_rect;
        detectContentRect(*surface, content_rect);
        if(content_rect.w != surface->w || content_rect.h != surface->h)
        {
            m_source_rect.x = static_cast<float>(content_rect.x);
            m_source_rect.y = static_cast<float>(content_rect.y);
            m_source_rect.w = static_cast<float>(content_rect.w);
            m_source_rect.h = static_cast<float>(content_rect.h);
        }
    }
    else if(_options.rect.has_value())
    {
        m_source_rect = _options.rect.value();
    }
    else
    {
        m_source_rect.x = .0f;
        m_source_rect.y = .0f;
        m_source_rect.w = static_cast<float>(surface->w);
        m_source_rect.h = static_cast<float>(surface->h);
    }
    m_desination_size.w = m_source_rect.w;
    m_desination_size.h = m_source_rect.h;
    m_texture = mp_renderer->createTexture(*surface, "Sprite");
    SDL_DestroySurface(surface);
    return true;
}

void Sprite::render(const SDL_FPoint & _point, const Rotation & _rotation, SDL_FlipMode _flip_mode)
{
    if(!isValid())
        return;
    SDL_FRect dest_rect = {.x = _point.x, .y = _point.y, .w = m_desination_size.w, .h = m_desination_size.h};
    mp_renderer->renderTexture(TextureRenderingData(dest_rect, m_texture, m_source_rect, _rotation, _flip_mode));
}
