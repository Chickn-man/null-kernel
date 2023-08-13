/*
#  main stuffs
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

#include "conio.h"
#include "screen.h"
#include "video/vga.h"

#include "io/pic.h"

#include "interrupts/interrupts.h"
#include "interrupts/handlers.h"

#include "shell.h"

int main() {
    vgaEnableCursor();
    setColor(0x07);

    screenFill(0, 0, 80, 25, ' ', 0x07);

    uint8_t idtrBuf[0x1000]; // TODO allocate this at runtime
    idtr.limit = 0x0fff;
    idtr.offset = &idtrBuf;

    setIdtGate((void*)pageFaultHandler, 0xe, INTR, 0x08);
    setIdtGate((void*)doubleFaultHandler, 0x8, INTR, 0x08);
    setIdtGate((void*)genProcFaultHandler, 0xd, INTR, 0x08);
    setIdtGate((void*)keyboardHandler, 0x21, INTR, 0x08);

    asm ("lidt %0" : : "m" (idtr));

    remapPic();
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);

    asm ("sti");

    shell();

    while (1) asm volatile ("hlt");
}