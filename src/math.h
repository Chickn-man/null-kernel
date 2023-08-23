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

#ifndef _MATH_H
#define _MATH_H

#include <stdint.h>
#include <stddef.h>

double sqrt(double number);
double rand();
int roundd(double x);

typedef struct {
  size_t size;
  uint8_t* buffer;
} bitmap;

// returns the indexth bit of map
// returns 255 if index too large
// returns 254 if map is invalid
uint8_t bitmapGet(uint64_t index, bitmap* map);

// returns value if setting was succesful
// returns 255 if index too large
// returns 254 if map is invalid
// returns 253 if value is invalid
uint8_t bitmapSet(uint64_t index, bitmap* map, uint8_t value);

#endif // !defined _MATH_H