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

#ifndef _CONIO_H
#define _CONIO_H

#include <stdint.h>

int initConio(uint16_t mode);

int wherex();
int wherey();
void gotox(int x);
void gotoy(int y);
void gotoxy(int x, int y);

void cputs(char *s);
void cputc(char c);
void cputc2(char c);
char cgetc();

void clrscr();

#endif // !defined _CONIO_H