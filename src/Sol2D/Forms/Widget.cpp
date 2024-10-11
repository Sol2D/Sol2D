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

#include <Sol2D/Forms/Widget.h>

using namespace Sol2D::Forms;

void Widget::step(const StepState & /*_state*/)
{
    renderBorder();
}

void Widget::renderBorder()
{
    float brd_width = this->border_width[m_state];
    if(brd_width <= .0f)
    {
        return;
    }
    Color brd_color = this->border_color[m_state];
    SDL_SetRenderDrawColor(&mr_renderer, brd_color.r, brd_color.g, brd_color.b, brd_color.a);

    const float x0 = m_x.getPixels(mr_parent.getWidth());
    const float y0 = m_y.getPixels(mr_parent.getHeight());

    if(brd_width == 1.0f)
    {
        SDL_FRect rect =
        {
            .x = x0,
            .y = y0,
            .w = m_width.getPixels(mr_parent.getWidth()),
            .h = m_height.getPixels(mr_parent.getHeight())
        };
        SDL_RenderRect(&mr_renderer, &rect);
    }
    else
    {
        float width = m_width.getPixels(mr_parent.getWidth());
        float height = m_height.getPixels(mr_parent.getHeight());
        SDL_FRect rect =
        {
            .x = x0,
            .y = y0,
            .w = brd_width,
            .h = height
        };
        SDL_RenderFillRect(&mr_renderer, &rect);
        rect.x = x0 + width - brd_width;
        SDL_RenderFillRect(&mr_renderer, &rect);
        rect.x = x0;
        rect.w = width;
        rect.h = brd_width;
        SDL_RenderFillRect(&mr_renderer, &rect);
        rect.y = y0 + height - brd_width;
        SDL_RenderFillRect(&mr_renderer, &rect);
    }
}
