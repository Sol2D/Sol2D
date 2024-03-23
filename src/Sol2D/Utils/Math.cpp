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

#include <Sol2D/Utils/Math.h>

void Sol2D::Utils::rotateVectors(std::vector<SDL_FPoint> & _vectors, double _angle_rad)
{
    VectorRotator rotator(_angle_rad);
    for(size_t i = 0; i < _vectors.size(); ++i)
        _vectors[i] = rotator.rotate(_vectors[i]);
}
