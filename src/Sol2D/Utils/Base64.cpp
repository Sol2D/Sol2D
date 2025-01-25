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

#include <Sol2D/Utils/Base64.h>
#include <cstring>

#define INVALID_INDEX ((uint8_t)-1)

namespace {

static const char gp_b64_alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline size_t getLengthWithoutPads(const std::string & _base64)
{
    size_t length = _base64.length();
    for(; length > 0 && '=' == _base64[length - 1]; --length);
    return length;
}

inline uint8_t getBase64AlphabetIndex(const char _symbol)
{
    const char * ptr = std::strchr(gp_b64_alphabet, _symbol);
    return ptr == nullptr ? INVALID_INDEX : static_cast<uint8_t>(ptr - gp_b64_alphabet);
}

} // namespace

std::shared_ptr<std::vector<uint8_t>> Sol2D::Utils::decodeBase64(const std::string & _base64)
{
    const uint16_t index_size_in_bits = 6;
    const uint16_t byte_size_in_bits = 8;
    const uint16_t byte_mask = 0b11111111;

    size_t input_length = getLengthWithoutPads(_base64);
    size_t output_length = input_length * index_size_in_bits / byte_size_in_bits;
    std::shared_ptr<std::vector<uint8_t>> output(new std::vector<uint8_t>(output_length));
    size_t output_index = 0;
    uint16_t buffer = 0;
    uint16_t bits = 0;

    for(size_t i = 0; i < input_length; ++i)
    {
        uint8_t alphabet_index = getBase64AlphabetIndex(_base64[i]);
        if(alphabet_index == INVALID_INDEX)
            return nullptr;

        buffer = (buffer << index_size_in_bits) | alphabet_index;
        bits += index_size_in_bits;
        if(bits >= byte_size_in_bits)
        {
            bits -= byte_size_in_bits;
            (*output)[output_index++] = (buffer >> bits) & byte_mask;
        }
    }

    return output;
}
