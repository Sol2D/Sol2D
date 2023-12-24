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

#include <Sol2D/Math.h>
#include <numbers>
#include <cmath>

double Sol2D::degreesToRadians(double _degrees)
{
    constexpr double deg_360 = 360.0;
    double deg = _degrees;
    if(deg > deg_360) deg -= deg_360 * floor(deg / deg_360);
    if(deg < 0) deg += deg_360 * floor(fabs(deg) / deg_360);
    return (deg * std::numbers::pi) / 180;
}

void Sol2D::rotateVectors(std::vector<SDL_FPoint> & _vectors, double _angle_rad)
{
    const double sine = sin(_angle_rad);
    const double cosine = cos(_angle_rad);
    const size_t size = _vectors.size();
    float x, y;
    for(size_t i = 0; i < size; ++i)
    {
        x = _vectors[i].x;
        y = _vectors[i].y;
        _vectors[i].x = cosine * x - sine * y;
        _vectors[i].y  = sine * x + cosine * y;
    }
}
