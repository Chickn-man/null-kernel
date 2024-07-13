/*
#  framebuffer functions for the null Kernel
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

#ifndef _VIDEO_FRAMEBUFFER_H
#define _VIDEO_FRAMEBUFFER_H

#include <stdint.h>

#include "../multiboot/mbi.h"

int fb_pixel(uint64_t x, uint64_t y, uint64_t color, mbiFramebuffer *fb);

#endif //!defined _VIDEO_FRAMEBUFFER_H