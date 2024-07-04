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

#include "multiboot/mbi.h"

#include "conio.h"
#include "string.h"
#include "screen.h"
#include "video/vga.h"

#include "paging/paging.h"

#include "io/pic.h"

#include "interrupts/interrupts.h"
#include "interrupts/handlers.h"

#include "shell.h"

extern void *kernelStart;
extern void *kernelEnd;

void *mbi;

int main() {
    asm("cli");

    cputs("\n\r");
    cputs("main()\n\r");

    char buffer[64];

    // lock memory thats in use
    uint8_t pb[0x200]; // this only gives us access to the first 2mb of memory, we'll reallocate this later
    pageBitmap.size = 0x1000;
    pageBitmap.buffer = (uint8_t*)&pb;

    lockPages(0, 256); // lock the first 1mb

    lockPages(kernelStart, ((kernelEnd - kernelStart) >> 12) + 1);

    mbiMap *memoryMap = getMbiEntry(mbi, mbir.memoryMap);
    if (memoryMap->type != mbir.memoryMap) {cputs("Error: issue parsing mbi"); while (1) asm volatile("hlt");}

    for (uint64_t i = 16; i < memoryMap->size; i += memoryMap->entrySize) {
        if (((mapEntry*)(memoryMap + i))->type != memMap.free) {
            lockPages((void *)(((mapEntry*)(memoryMap + i))->addr), (((mapEntry*)(memoryMap + i))->size >> 12) + 1);
        }
    }

    // map things
    cputs("map pages\n\r");

    mapPages(0, 0, 256, 1); // map the first 1mb to itself
    mapPages(&kernelStart, &kernelStart, (uint64_t)((&kernelEnd - &kernelStart) >> 12) + 1, 1);

    // IDT crap
    uint8_t idtrBuf[0x1000]; // TODO allocate this at runtime
    idtr.limit = 0x0fff;
    idtr.offset = (uint64_t)&idtrBuf;

    setIdtGate((void *)pageFaultHandler, 0xe, INTR, 0x08);
    setIdtGate((void *)doubleFaultHandler, 0x8, INTR, 0x08);
    setIdtGate((void *)genProcFaultHandler, 0xd, INTR, 0x08);
    setIdtGate((void *)keyboardHandler, 0x21, INTR, 0x08);

    asm("lidt %0" :: "m"(idtr));

    remapPic();
    outb(PIC1_DATA, 0b11111001);
    outb(PIC2_DATA, 0b11101111);

    

    asm("sti");

    // now we can do user stuffs
    vgaEnableCursor();
    setColor(0x07);

    //screenFill(0, 0, 80, 25, ' ', 0x07);

    shell();

    while (1) asm volatile("hlt");
}