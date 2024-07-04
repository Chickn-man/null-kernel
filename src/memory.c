/*
#  memory manipulation stuffs
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

#include "memory.h"

void memcpy(void *dst, void *src, uint64_t len) {
    int i;

    for (i = 0; i < len; i++) ((char *)dst)[i] = ((char *)src)[i]; 
}

void *memset(void *addr, int value, uint64_t len) {
    for (uint64_t i = 0; i < len; i++) {
        ((char *)addr)[i] = value;
    }
}