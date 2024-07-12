/*
#  serial debuging functions for the null Kernel
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
#include "serial.h"
#include "io/pic.h"
#include "screen.h"
#include "video/vga.h"
#include "io/hid/keyboard.h"

#include <stdint.h>
#include <stddef.h>

character *s_screen = (character *)0xb8000;

size_t s_cols = 80;
size_t s_rows = 25;

int s_cursorX = 0;
int s_cursorY = 0;

uint8_t s_defaultColor = 0x07;


int s_wherex() {
    return s_cursorX;
}

int s_wherey() {
    return s_cursorY;
}

void s_gotox(int x) {
    if (x >= s_cols) return;
    s_cursorX = x;
    vgaUpdateCursor(s_cursorX, s_cursorY);
}

void s_gotoy(int y) {
    if (y >= s_rows) return;
    s_cursorY = y;
    vgaUpdateCursor(s_cursorX, s_cursorY);
}

void s_gotoxy(int x, int y) {
    if (x >= s_cols) return;
    if (y >= s_rows) return;
    s_cursorX = x;
    s_cursorY = y;
    vgaUpdateCursor(s_cursorX, s_cursorY);
}

void s_cputs(char *s) {
    for (int i = 0; s[i]; i++) s_cputc(s[i]);
}

void s_cputc(char c) {
    outb(SERIAL_PORT, c);
}

char s_cgetc() {
    char c = keyBuffer[0];
    inb(SERIAL_PORT);
    return (c);
}

/* puts a character c at the cursor position ignoring \n and \r */
void s_cputc2(char c) {
    if (c == '\n' || c == '\r') return;

    s_cputc(c);
}

void s_clrscr() {
    screenFill(0, 0, s_cols, s_rows, ' ', s_defaultColor);

    s_gotoxy(0, 0);
}

int init_serial() {
   outb(SERIAL_PORT + 1, 0x00);    // Disable all interrupts
   outb(SERIAL_PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(SERIAL_PORT + 0, 0x0C);    // Set divisor to 12 (lo byte) 9600 baud
   outb(SERIAL_PORT + 1, 0x00);    //                  (hi byte)
   outb(SERIAL_PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(SERIAL_PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(SERIAL_PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(SERIAL_PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(SERIAL_PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(inb(SERIAL_PORT + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(SERIAL_PORT + 4, 0x0F);
   return 0;
}
