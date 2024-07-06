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

#include <Sol2D/Forms/Widget.h>
#include <Sol2D/Forms/TextAlignment.h>
#include <Sol2D/SDL/SDL.h>
#include <string>

namespace Sol2D::Forms {

class Label : public Widget
{
    struct SideEffect;
    friend struct SideEffect;

public:
    explicit Label(const Canvas & _parent, const std::string & _text, SDL_Renderer & _renderer);
    ~Label() override;
    bool setText(const std::string & _text);
    const std::string & getText() const;
    bool setState(WidgetState _state) override;
    void render(const RenderState & _state) override;

private:
    SideEffect * mp_side_effect;
    std::string m_text;
    SDL_Texture * mp_texture;
    float m_texture_width;
    float m_texture_height;

public:
    WidgetProperty<HorizontalTextAlignment> horizontal_text_alignment;
    WidgetProperty<VerticalTextAlignment> vertical_text_alignment;

private:
    void resetTexture();
    void createTexture(TTF_Font * _font);
};

inline const std::string & Label::getText() const
{
    return m_text;
}

} // namespace Sol2D::Forms
