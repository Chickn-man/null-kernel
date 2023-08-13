/*
#  interrupt handlers for the null Kernel
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

#include "handlers.h"
#include "../conio.h"
#include "../io/pic.h"

__attribute__((interrupt)) void pageFaultHandler(struct interruptFrame* frame) {
    gotoxy(0, 0);
    cputs("Page Fault");

    //halt cpu
    for (;;) {
        asm volatile("hlt");
    }
}

__attribute__((interrupt)) void doubleFaultHandler(struct interruptFrame* frame) {
    gotoxy(0, 0);
    cputs("Double Fault");

    //halt cpu
    for (;;) {
        asm volatile("hlt");
    }
}

__attribute__((interrupt)) void genProcFaultHandler(struct interruptFrame* frame) {
    gotoxy(0, 0);
    cputs("General Protection Fault");

    //halt cpu
    for (;;) {
        asm volatile("hlt");
    }
}

#include "../io/hid/keyboard.h"

uint8_t leftShiftDown;
uint8_t rightShiftDown;
uint8_t mod;

__attribute__((interrupt)) void keyboardHandler(struct interruptFrame* frame) {
    uint8_t scancode = inb(0x60);
    picEndMaster();

    switch (scancode) {
        case 0xe0:
            mod = 1;
            return;
        case usKeyLeftShift:
            leftShiftDown = 1;
            return;
        case usKeyLeftShift + 0x80:
            leftShiftDown = 0;
            break;
        case usKeyRightShift:
            rightShiftDown = 1;
            return;
        case usKeyRightShift + 0x80:
            rightShiftDown = 0;
            break;    
    }

    if (scancode > 58) return;

    int i;
    for (i = 0; keyBuffer[i] & i < 64; i++);

    if (rightShiftDown | leftShiftDown) {
        keyBuffer[i] = usKeyUpper[scancode];
        return;
    }

    keyBuffer[i] = usKeyLower[scancode];
    //cputs(keyBuffer);
    //cputs("\n\r");
    //cputc(usKeyLower[scancode]);
}