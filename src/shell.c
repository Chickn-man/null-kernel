/*
#  null Kernel debug shell
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

#include "shell.h"
#include "conio.h"
#include "io/hid/keyboard.h"
#include "string.h"

void shell() {
    char c;

    char buffer[128];
    char input[128];
    char command[128];

    goto VERSION;

    while (1) {
        c = cgetc();
        if (c == '\b') strdelc(input), cputs("\b \b"), c = 0; else cputc(c);

        if (c == '\r' || c == '\n') {
            if (c == '\r') cputc('\n');
            cputc('\r');

            if (strlen(input) != 0) {
                strcpy(command, strtok(input, " "));
CHAIN:
                if (strcmp(command, "cls") == 0) {
                    clrscr();

                } else if (strcmp(command, "ascii") == 0) {
                    for (int i = 0; i < 255; i++) cputc2(i);
                    cputc('\n');
                    cputc('\r');

                } else if (strcmp(command, "version") == 0) {
VERSION:            cputs("Null ");
                    cputs(itoa(VERSION_REL, buffer, 10));
                    cputc('.');
                    cputs(itoa(VERSION_MAJ, buffer, 10));
                    cputc('.');
                    cputs(itoa(VERSION_MIN, buffer, 10));
                    cputc('.');
                    cputs(itoa(VERSION_FIX, buffer, 10));
                    cputc('\n');
                    cputc('\r');

                } else if (strcmp(command, "ver") == 0) {
                    goto VERSION;

                } else if (strcmp(command, "peek") == 0) {
                    // grab address from input, read value at addr and print value in hex
                    cputs(itoa(*(unsigned char*)hextoi(strtok(0, " ")), buffer, 16));
                    cputc('\n');
                    cputc('\r');

                } else if (strcmp(command, "poke") == 0) {
                    // grab address from input, grab hex value from input, and write value to address
                    *(unsigned char *)hextoi(strtok(0, " ")) = (unsigned char)hextoi(strtok(0, " "));

                } else {
                    cputs("command not found\n\r");
                }

                if (strcmp(strtok(0, " "), "&&") == 0) {strcpy(command, strtok(0, " ")); goto CHAIN;}
            }
            for (int i = 0; i < 128; i++) input[i] = 0;
            for (int i = 0; i < 128; i++) command[i] = 0;
             
            cputc('>');
            
        } else strcatc(input, c);
    }
}