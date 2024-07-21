/*
#  vt100 terminal emulator for the null Kernel
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

#include "../memory.h"

#include "terminal.h"
#include "framebuffer.h"
#include "../font/psf.h"

#include "../io/hid/keyboard.h"

#include <stdint.h>
#include <stddef.h>

size_t term_cols = 0;
size_t term_rows = 0;

int term_cursorX = 0;
int term_cursorY = 0;

int term_inEscSeq = 0;
int term_escSeqLen = 0;
char term_escSeq[ESC_SEQ_MAX_LEN];

BITMAP_FONT *terminalFont;
mbiFramebuffer *terminalFramebuffer;

int initTerminal(BITMAP_FONT *font, mbiFramebuffer *fb) {
    terminalFont = font;
    terminalFramebuffer = fb;

    term_cols = fb->width / font->width;
    term_rows = fb->height / font->height;
}

char tgetc() {
    char c = keyBuffer[0];
    if (!c) return c;
    memcpy(keyBuffer, keyBuffer + 1, 62);
    keyBuffer[63] = 0;
    return c;
}

int term_HandleEscapeSequence() {
    if (term_escSeq[1] == '[') { // CSI (Control Sequence Introducer)
        int args[2] = {0, 0};
        int arg_count = 0;

        char *p = term_escSeq + 2;

        while (*p && (*p != 'm' && *p != 'H' && *p != 'A' && *p != 'B' && *p != 'C' && *p != 'D' && *p != 'J')) {
            if (*p >= '0' && *p <= '9') {
                args[arg_count] = args[arg_count] * 10 + (*p - '0');
            } else if (*p == ';') {
                arg_count++;
            }
            p++;
        }

        switch (*p) {
            case 'A': // Cursor Up
                term_cursorY = (term_cursorY >= args[0]) ? term_cursorY - args[0] : 0;
                break;
            case 'B': // Cursor Down
                term_cursorY = (term_cursorY + args[0] < term_rows) ? term_cursorY + args[0] : term_rows - 1;
                break;
            case 'C': // Cursor Forward
                term_cursorX = (term_cursorX + args[0] < term_cols) ? term_cursorX + args[0] : term_cols - 1;
                break;
            case 'D': // Cursor Backward
                term_cursorX = (term_cursorX >= args[0]) ? term_cursorX - args[0] : 0;
                break;
            case 'H': // Cursor Position
                term_cursorX = (args[1] > 0 && args[1] <= term_cols) ? args[1] - 1 : 0;
                term_cursorY = (args[0] > 0 && args[0] <= term_rows) ? args[0] - 1 : 0;
                break;
            case 'J': // Erase in Display
                if (args[0] == 2) { // Clear entire screen
                    for (int y = 0; y < term_rows; y++) {
                        for (int x = 0; x < term_cols; x++) {
                            renderChar(x * terminalFont->width, y * terminalFont->height, ' ', 0x000000, terminalFont, terminalFramebuffer);
                        }
                    }
                    term_cursorX = 0;
                    term_cursorY = 0;
                }
                break;
            // Add more escape sequence cases as needed
        }
    }
    term_inEscSeq = 0;
    term_escSeqLen = 0;
}

void tputc(char c) {
    if (term_inEscSeq) {
        if (term_escSeqLen < ESC_SEQ_MAX_LEN - 1) {
            term_escSeq[term_escSeqLen++] = c;
            term_escSeq[term_escSeqLen] = '\0';
            if ((c >= '@' && c <= '~') && c != '[') { // Final character of an escape sequence
                term_HandleEscapeSequence();
            }
        } else {
            term_inEscSeq = 0; // Invalid sequence, reset
        }
        return;
    }

    if (!c) return;

    switch (c) {
        case '\n':
            term_cursorY++;
            break;
        case '\r':
            term_cursorX = 0; 
            break;
        case '\b':
            term_cursorX--;
            if (term_cursorX < 0) term_cursorX = 0;
            break;
        case '\e':
            term_escSeq[0] = c;

            term_inEscSeq = 1;
            term_escSeqLen = 1;
            break;
        default:
            renderChar(term_cursorX * terminalFont->width, term_cursorY * terminalFont->height, c, 0xffffff, terminalFont, terminalFramebuffer);
            term_cursorX++;
            break;
    }

    if (term_cursorX >= term_cols) term_cursorX = 0, term_cursorY++;
    if (term_cursorY >= term_rows) {
        term_cursorY = term_rows - 1;
        for (int yi = 0; yi < terminalFont->height * (term_rows - 1); yi++) {
            for (int xi = 0; xi < terminalFramebuffer->width * (terminalFramebuffer->bpp / 8); xi++) {
                ((uint8_t *)terminalFramebuffer->buffer)[xi + yi * terminalFramebuffer->pitch] = ((uint8_t *)terminalFramebuffer->buffer)[xi + (yi + 16) * terminalFramebuffer->pitch];
            }
        }
        for (int yi = terminalFont->height * (term_rows - 1); yi < terminalFont->height * term_rows; yi++) {
            for (int xi = 0; xi < terminalFramebuffer->width * (terminalFramebuffer->bpp / 8); xi++) {
                ((uint8_t *)terminalFramebuffer->buffer)[xi + yi * terminalFramebuffer->pitch] = 0;
            }
        }
    }

    //vgaUpdateCursor(term_cursorX, term_cursorY);
}

void tputcr(char c) { // tputc but no escape codes
    renderChar(term_cursorX * terminalFont->width, term_cursorY * terminalFont->height, c, 0xffffff, terminalFont, terminalFramebuffer);
    term_cursorX++;

    if (term_cursorX >= term_cols) term_cursorX = 0, term_cursorY++;
    if (term_cursorY >= term_rows) {
        term_cursorY = term_rows - 1;
        for (int yi = 0; yi < terminalFont->height * (term_rows - 1); yi++) {
            for (int xi = 0; xi < terminalFramebuffer->width * (terminalFramebuffer->bpp / 8); xi++) {
                ((uint8_t *)terminalFramebuffer->buffer)[xi + yi * terminalFramebuffer->pitch] = ((uint8_t *)terminalFramebuffer->buffer)[xi + (yi + 16) * terminalFramebuffer->pitch];
            }
        }
        for (int yi = terminalFont->height * (term_rows - 1); yi < terminalFont->height * term_rows; yi++) {
            for (int xi = 0; xi < terminalFramebuffer->width * (terminalFramebuffer->bpp / 8); xi++) {
                ((uint8_t *)terminalFramebuffer->buffer)[xi + yi * terminalFramebuffer->pitch] = 0;
            }
        }
    }
}