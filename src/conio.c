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
#include "video/terminal.h"
#include "io/pic.h"

#include <stdint.h>
#include <stddef.h>

uint32_t outputMode = 0; 
// Possible Modes:
// 0 : No Output
// 1 : Terminal Emu
// 0x3F8 : COM1
// 0x2F8 : COM2
// 0x3E8 : COM3
// 0x2E8 : COM4
// 0x5F8 : COM5
// 0x4F8 : COM6
// 0x5E8 : COM7
// 0x4E8 : COM8

int initConio(uint16_t mode) { // ^^ mode can be any valid output mode ^^
    outputMode = mode;
}

int wherex() {
    return 0; // ansi cursor get
}

int wherey() {
    return 0; // ansi cursor get
} 

void gotox(int x) {
    // ansi cursor move
}

void gotoy(int y) {
    // ansi cursor move
}

void gotoxy(int x, int y) {
    // ansi cursor move
}

void cputs(char *s) {
    for (int i = 0; s[i]; i++) cputc(s[i]);
}

void cputc(char c) {
    if (outputMode == 1) {
        tputc(c);
    } else {
        outb(outputMode, c);
    }
}

char cgetc() {
    /*char c = keyBuffer[0];
    if (!c) return c;
    memcpy(keyBuffer, keyBuffer + 1, 62);
    keyBuffer[63] = 0;*/
    
    if (outputMode == 1) {
        return tgetc();
    } else {
        if (outputMode) return inb(outputMode);
    }

    return 0;

}

/* puts a character c at the cursor position ignoring escape codes */
void cputc2(char c) {
    tputcr(c);
}

void clrscr() {
    cputs("\e[2J\e[H");
}

