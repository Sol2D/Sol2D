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

#include <Sol2D/Utils/Zlib.h>
#include <zlib.h>

std::shared_ptr<std::vector<uint8_t>> Sol2D::Utils::zlibDecompress(
    ZlibAlgorithm _algorithm, const std::vector<uint8_t> & _data
)
{
    z_stream stream = {};
    stream.next_in = const_cast<Bytef *>(_data.data());
    stream.avail_in = static_cast<uint32_t>(_data.size());
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    if(inflateInit2(&stream, _algorithm == ZlibAlgorithm::Zlib ? MAX_WBITS : MAX_WBITS + 16) != Z_OK)
        return nullptr;

    static const uint32_t buffer_size = 16384;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);

    std::shared_ptr<std::vector<uint8_t>> output(new std::vector<uint8_t>);
    do
    {
        stream.avail_out = buffer_size;
        stream.next_out = buffer.get();
        switch(inflate(&stream, Z_NO_FLUSH))
        {
        case Z_OK:
        case Z_STREAM_END:
            break;
        default:
            inflateEnd(&stream);
            return nullptr;
        }
        uint8_t * b = buffer.get();
        output->insert(output->end(), b, &b[buffer_size - stream.avail_out]);
    } while(stream.avail_out == 0);
    inflateEnd(&stream);
    return output;
}
