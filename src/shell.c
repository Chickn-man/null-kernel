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

#define STRINGIZE(foo) #foo
#define S(bar) STRINGIZE(bar)
#define VER_TEXT "Null " S(VERSION_REL) "." S(VERSION_MAJ) "." S(VERSION_MIN) "." S(VERSION_FIX) "\n\r"
#define HELP_TEXT "Null " S(VERSION_REL) "." S(VERSION_MAJ) "." S(VERSION_MIN) "." S(VERSION_FIX) "\n\r" \
"Builtin Kernel Shell\n\n\r" \
"Commands:\n\r"\
"help : print this help\n\r" \
"cls : clear screen\n\r" \
"version/ver : print kernel version\n\r" \
"ascii : print all ascii characters\n\r" \
"peek [address] : print byte at address, raw hex value\n\r" \
"peek [byte] [address] : store byte at address, raw hex values\n\r" \
"\n\r"


void shell() {
    char c;

    char buffer[128];
    char input[128];
    char command[128];

    clrscr();
    cputs(HELP_TEXT);
    cputc('>');

    while (1) {
        c = cgetc();
        if (!c) continue;
        if (c == '\b') strdelc(input), cputs("\b \b"), c = 0; else cputc(c);

        if (c == '\r' || c == '\n') {
            if (c == '\r') cputc('\n');
            cputc('\r');

            if (strlen(input) != 0) {
                strcpy(command, strtok(input, " "));
CHAIN:
                if (strcmp(command, "help") == 0) {
                    cputs(HELP_TEXT);

                } else if (strcmp(command, "cls") == 0) {
                    clrscr();

                } else if (strcmp(command, "ascii") == 0) {
                    for (int i = 0; i < 255; i++) cputc2(i);
                    cputc('\n');
                    cputc('\r');

                } else if (strcmp(command, "version") == 0) {
VERSION:            cputs(VER_TEXT);

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