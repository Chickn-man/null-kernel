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

#ifndef _SERIAL_DEBUG_H
#define _SERIAL_DEBUG_H

#define SERIAL_PORT 0x3f8

int s_wherex();
int s_wherey();
void s_gotox(int x);
void s_gotoy(int y);
void s_gotoxy(int x, int y);

void s_cputs(char *s);
void s_cputc(char c);
void s_cputc2(char c);
char s_cgetc();

void s_clrscr();

int init_serial();

#endif // !defined _SERIAL_DEBUG_Hs_