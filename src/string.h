/*
#  string functions for the null Kernel
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
#  but WITHOUT ANY WARRANTY without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program. If not, see <https://www.gnu.org/licenses/>.
#
#############################################################################
*/

#ifndef _STRING2_H
#define _STRING2_H

unsigned short strlen(char *string);
void strcpy(char *dst, char *src);
void strcat(char *dst, char *src);
void strcatc(char *dst, char c);
int strcmp(char *s1, char *s2);
void strdelc(char *str);
char *strchr(char *s, int c);
char *strtok(char *s, char *delim);
char *strupr(char *s);
char *strlwr(char *s);

char toupper(char c);
char tolower(char c);

unsigned int hextoi(char *str);
char *itohex(unsigned int num);
int atoi(char *str);
char *itoa(int n, char *buffer, int radix);

#endif