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

#include "framebuffer.h"

int fb_pixel(uint64_t x, uint64_t y, uint64_t color, mbiFramebuffer *fb) {
    for (int i = 0; i < fb->bpp / 8; i++) {
        ((uint8_t *)fb->buffer)[x * (fb->bpp / 8) + y * fb->pitch + i] = (uint8_t)(color >> i * 8);
    }
}

void renderChar(uint64_t x, uint64_t y, char c, uint32_t color, BITMAP_FONT *font, mbiFramebuffer *fb) {
    for (int yi = 0; yi < font->height; yi++) {
        for (int xi = 0; xi < font->width; xi++) {
            if (font->buffer[yi + (c * font->height)] >> ((font->width - 1) - xi) & 1 ) { // check if pixel is set in font
                fb_pixel(x + xi, y + yi, color, fb);
            } else {
                fb_pixel(x + xi, y + yi, 0x000000, fb);
            }
        }       
    }
}