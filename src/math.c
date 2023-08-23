/*
#  math
#
##############################################################################
#
#  Copyright (C) 2023 Keegan Powers
#
#  This file is part of the null Kernel
#
#  The null Kernel is free software: you can redistribute it
#  and/or modify it under the terms of the GNU General Public
#  License as published by the Free Software Foundation, either
#  version 3 of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#
##############################################################################
*/

#include "math.h"

uint8_t bitmapGet(uint64_t index, bitmap* map) {
    if (index > map->size * 8) return 255; // index too big
    if (!map) return 254; // map is null pointer (invalid)

    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b00000001 << bitIndex;

    if ((map->buffer[byteIndex] & bitIndexer) > 0){
        return 1;
    }

    return 0;
}

uint8_t bitmapSet(uint64_t index, bitmap* map, uint8_t value) {
    if (index > map->size * 8) return 255; // index too big
    if (!map) return 254; // map is null pointer (invalid)
    if (value > 1) return 253; // value too big (invalid)
    
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t bitIndexer = 0b00000001 << bitIndex;

    map->buffer[byteIndex] &= ~bitIndexer;

    if (value) {
        map->buffer[byteIndex] |= bitIndexer;
    }

    return value;
}