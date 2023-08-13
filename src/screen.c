/*
#  screen stuffs
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

#include "screen.h"
#include "stdint.h"
#include "stddef.h"

character *screen = (character *)0xb8000;

size_t cols = 80;
size_t rows = 25;

int cursorX = 0;
int cursorY = 0;

uint8_t defaultColor = 0x07;

// fills a rectangular region on the screen
void screenFill(unsigned char x, unsigned char y, unsigned char w, unsigned char h, unsigned char c, uint8_t color) {
    for (int i = 0; i < h * w; i++) {
        screen[(((y * cols) + ((i / w) * cols)) + (x + (i % w)))] = (character){c, color};
    }
}

// moves a rectangular region from one position to another on the screen
void screenShift(unsigned char sx, unsigned char sy, unsigned char dx, unsigned char dy, unsigned char w, unsigned char h) {
    character temp[2000];

    if (sx > cols | dx > cols | w > cols | sy > rows | dy > rows | h > rows) return;

    for (int i = 0; i < (h * cols) + w; i++) {
        temp[i] = screen[(((sy * cols) + ((i / w) * cols)) + (sx + (i % w)))];
    }

    screenFill(sx, sy, w, h, ' ', defaultColor);

    for (int i = 0; i < (h * cols) + w; i++) {
        screen[(((dy * cols) + ((i / w) * cols)) + (dx + (i % w)))] = temp[i];
    }
}

void setColor(uint8_t color) {
    defaultColor = color;
}