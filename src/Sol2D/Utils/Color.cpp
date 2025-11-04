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

#include <Sol2D/Utils/Color.h>

std::optional<SDL_Color> Sol2D::Utils::tryParseHexColorString(const std::string & _str)
{
    std::string r_str, g_str, b_str;
    switch(_str.length())
    {
    case 4:
        r_str = { _str[1], _str[1], '\0' };
        g_str = { _str[2], _str[2], '\0' };
        b_str = { _str[3], _str[3], '\0' };
        break;
    case 7:
        r_str = _str.substr(1, 2);
        g_str = _str.substr(3, 2);
        b_str = _str.substr(5, 2);
        break;
    default:
        return std::nullopt;
    }

    SDL_Color color = {};
    color.a = 255;
    size_t pos = 0;
    color.r = static_cast<Uint8>(std::stoi(r_str, &pos, 16));
    if(pos != 2) return std::nullopt;
    pos = 0;
    color.g = static_cast<Uint8>(std::stoi(g_str, &pos, 16));
    if(pos != 2) return std::nullopt;
    pos = 0;
    color.b = static_cast<Uint8>(std::stoi(b_str, &pos, 16));
    if(pos != 2) return std::nullopt;
    return color;
}
