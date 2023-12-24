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

#include <Sol2D/Utils/String.h>
#include <cstring>

std::string Sol2D::Utils::trimString(const char * _str)
{
    if(_str == nullptr)
        return std::string();
    size_t len = strlen(_str);
    const char * begin = _str;
    const char * end = &_str[len - 1];
    for(; isspace(*begin); ++begin)
        ;
    if(*begin == '\0')
        return std::string();
    for(; isspace(*end); --end)
        ;
    return std::string(begin, end - begin + 1);
}
