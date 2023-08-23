/*
#  conio functions for the null Kernel
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
#include "conio.h"
#include "screen.h"
#include "video/vga.h"
#include "io/hid/keyboard.h"

#include <stdint.h>
#include <stddef.h>

extern character *screen;

extern size_t cols;
extern size_t rows;

extern int cursorX;
extern int cursorY;

extern uint8_t defaultColor;

int wherex() {
    return cursorX;
}

int wherey() {
    return cursorY;
}

void gotox(int x) {
    if (x >= cols) return;
    cursorX = x;
    vgaUpdateCursor(cursorX, cursorY);
}

void gotoy(int y) {
    if (y >= rows) return;
    cursorY = y;
    vgaUpdateCursor(cursorX, cursorY);
}

void gotoxy(int x, int y) {
    if (x >= cols) return;
    if (y >= rows) return;
    cursorX = x;
    cursorY = y;
    vgaUpdateCursor(cursorX, cursorY);
}

void cputs(char *s) {
    for (int i = 0; s[i]; i++) cputc(s[i]);
}

// crude prototype
void cputc(char c) {
    if (!c) return;

    switch (c) {
        case '\n':
            cursorY++;
            break;
        case '\r':
            cursorX = 0; 
            break;
        case '\b':
            cursorX--;
            if (cursorX < 0) cursorX = 0;
            break;
        default:
            screen[(cols * cursorY) + cursorX] = (character){c, defaultColor};
            cursorX++;
            break;
    }

    if (cursorX >= cols) cursorX = 0, cursorY++;
    if (cursorY >= rows) {
        cursorY = rows - 1;
        screenShift(0, 1, 0, 0, cols, rows - 1);
        screenFill(0, rows, cols, 1, ' ', defaultColor);
    }

    vgaUpdateCursor(cursorX, cursorY);
}

char cgetc() {
    char c = keyBuffer[0];
    memcpy(keyBuffer, keyBuffer + 1, 64);
    keyBuffer[63] = 0;
    return (c);
}

/* puts a character c at the cursor position ignoring \n and \r */
void cputc2(char c) {
    if (c == '\n' || c == '\r') return;

    cputc(c);
}

void clrscr() {
    screenFill(0, 0, cols, rows, ' ', defaultColor);

    gotoxy(0, 0);
}

