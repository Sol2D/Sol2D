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

#include <Sol2D/Exception.h>
#include <Sol2D/MediaLayer/MediaLayer.h>
#include <sstream>
#include <cstring>

namespace Sol2D {

class SDLException : public Exception
{
public:
    SDLException(const std::string & _message) :
        Exception(makeFullMessage(_message))
    {
    }

private:
    std::string makeFullMessage(const std::string & _base_message)
    {
        const char * sdl_error = SDL_GetError();
        if(sdl_error && std::strlen(sdl_error) > 0)
        {
            std::stringstream stream;
            stream << _base_message << std::endl << "SDL error message: " << sdl_error;
            return stream.str();
        }
        return _base_message;
    }
};

} // namespace Sol2D
