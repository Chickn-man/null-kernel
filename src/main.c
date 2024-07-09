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
#include "serial.h"
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

void print_page_map(uint64_t *pml4_base) {
    for (int pml4_index = 0; pml4_index < 512; pml4_index++) {
        if (pml4_base[pml4_index] & 1) { // Check if entry is present
            uint64_t *pdpt_base = (uint64_t *)(pml4_base[pml4_index] & ~0xFFF);
            for (int pdpt_index = 0; pdpt_index < 512; pdpt_index++) {
                if (pdpt_base[pdpt_index] & 1) { // Check if entry is present
                    uint64_t *pd_base = (uint64_t *)(pdpt_base[pdpt_index] & ~0xFFF);
                    for (int pd_index = 0; pd_index < 512; pd_index++) {
                        if (pd_base[pd_index] & 1) { // Check if entry is present
                            uint64_t *pt_base = (uint64_t *)(pd_base[pd_index] & ~0xFFF);
                            for (int pt_index = 0; pt_index < 512; pt_index++) {
                                if (pt_base[pt_index] & 1) { // Check if entry is present
                                    char address_str[17];
                                    char size_str[17];
                                    
                                    uint64_t address = (pml4_index << 39) | (pdpt_index << 30) | (pd_index << 21) | (pt_index << 12);
                                    uint64_t size = 4096; // Each page is 4KB

                                    s_cputs("Address: 0x");
                                    s_cputs(itoa(address, address_str, 16));
                                    s_cputs(", Size: 0x");
                                    s_cputs(itoa(size, size_str, 16));
                                    s_cputs("\r\n");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int main() {
    asm("cli");

    init_serial();

    cputs("\n\r");

    char buffer[64];

    uint64_t* pt = page_table_l4;

    // lock memory thats in use
    uint8_t pb[0x800]; // this only gives us access to the first 8mb of memory, we'll reallocate this later
    pageBitmap.size = 0x4000;
    pageBitmap.buffer = (uint8_t*)&pb;

    lockPages(0, 1000); // lock the first 4mb

    lockPages(kernelStart, ((kernelEnd - kernelStart) >> 12) + 1);

    mbiMap *memoryMap = getMbiEntry(mbi, mbir.memoryMap);
    if (memoryMap->type != mbir.memoryMap) {cputs("Error: issue parsing mbi"); while (1) asm volatile("hlt");}

    for (uint64_t i = 16; i < memoryMap->size; i += memoryMap->entrySize) {
        if (((mapEntry*)(memoryMap + i))->type != memMap.free) {
            lockPages((void *)(((mapEntry*)(memoryMap + i))->addr), (((mapEntry*)(memoryMap + i))->size >> 12) + 1);
        }
    }

    // map things
    mapPages(0, 0, 1024, 1); // map the first 4mb to itself
    //mapPages(&kernelStart, &kernelStart, (uint64_t)((&kernelEnd - &kernelStart) >> 12) + 1, 1);

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

    s_cputs("Hello, Serial!\n\r");

    //mapPage((void *)0x400000000, (void *)0x400000000, 1);
    //print_page_map(page_table_l4);


    /*mbiFramebuffer *mbFramebuffer = getMbiEntry(mbi, mbir.fb);
    if (mbFramebuffer) {
        lockPage(mbFramebuffer->buffer);
        mapPage(mbFramebuffer->buffer, mbFramebuffer->buffer, 1);
        //asm("cli");
        //asm volatile("hlt");
        //asm("sti");
        mbFramebuffer->buffer[1] = 0xffffffff;
        cputs("fb\n\r");
    }*/

    // now we can do user stuffs
    vgaEnableCursor();
    setColor(0x07);

    //screenFill(0, 0, 80, 25, ' ', 0x07);

    shell();

    while (1) asm volatile("hlt");
}
