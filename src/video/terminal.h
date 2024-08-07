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

#ifndef _VIDEO_TERMINAL_H
#define _VIDEO_TERMINAL_H

#include "framebuffer.h"
#include "../font/psf.h"

#define ESC_SEQ_MAX_LEN 16

int initTerminal(BITMAP_FONT *font, mbiFramebuffer *fb);

char tgetc();
void tputc(char c);
void tputcr(char c);

#endif // !defined _VIDEO_TERMINAL_H