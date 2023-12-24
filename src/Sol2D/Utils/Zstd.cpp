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

#include <Sol2D/Utils/Zstd.h>
#include <zstd.h>

std::shared_ptr<std::vector<uint8_t>> Sol2D::Utils::zstdDecompress(const std::vector<uint8_t> & _data)
{
    ZSTD_DCtx * dctx = ZSTD_createDCtx();
    if(dctx == nullptr)
        return nullptr;

    static const uint32_t buffer_size = 16384;
    std::unique_ptr<uint8_t[]> buffer(new uint8_t[buffer_size]);
    ZSTD_inBuffer input = { _data.data(), _data.size(), 0 };
    std::shared_ptr<std::vector<uint8_t>> output_buffer(new std::vector<uint8_t>);
    ZSTD_outBuffer output = { buffer.get(), buffer_size, 0 };

    do
    {
        if(ZSTD_isError(ZSTD_decompressStream(dctx, &output, &input)))
        {
            ZSTD_freeDCtx(dctx);
            return nullptr;
        }

        uint8_t * b = buffer.get();
        output_buffer->insert(output_buffer->end(), b, b + output.pos);
        output.pos = 0;
    } while(input.pos < input.size);

    ZSTD_freeDCtx(dctx);
    return output_buffer;
}
