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
#include "video/framebuffer.h"

#include "paging/paging.h"

#include "io/pic.h"

#include "interrupts/interrupts.h"
#include "interrupts/handlers.h"

#include "font/psf.h"

#include "shell.h"

extern void *kernelStart;
extern void *kernelEnd;

void *mbi;

void renderChar(uint64_t x, uint64_t y, char c, uint32_t color, BITMAP_FONT *font, mbiFramebuffer *fb);

int main() {
    asm("cli");

    init_serial();

    //cputs("\n\r");

    char buffer[64];

    uint64_t* pt = page_table_l4;

    // lock memory thats in use
    uint8_t pb[0x800]; // this only gives us access to the first 8mb of memory, we'll reallocate this later
    pageBitmap.size = 0x4000;
    pageBitmap.buffer = (uint8_t*)&pb;

    lockPages(0, 1000); // lock the first 4mb

    lockPages(kernelStart, ((kernelEnd - kernelStart) >> 12) + 1);

    mbiMap *memoryMap = getMbiEntry(mbi, mbir.memoryMap);
    if (memoryMap->type != mbir.memoryMap) {cputs("[KERNEL] Error: issue parsing mbi"); while (1) asm volatile("hlt");}

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


    BITMAP_FONT termFont;

    if (((PSF1_HEADER *)&_binary_fonts_default_psf_start)->magic = PSF1_MAGIC) {
        s_cputs("[KERNEL] Default font is PSF 1\n\r");
        termFont.width = 8;
        termFont.height = ((PSF1_HEADER *)&_binary_fonts_default_psf_start)->characterSize;
        termFont.buffer = (char *)(&_binary_fonts_default_psf_start + sizeof(PSF1_HEADER));
        termFont.size = &_binary_fonts_default_psf_end - termFont.buffer;

    } else if (((PSF2_HEADER *)&_binary_fonts_default_psf_start)->magic = PSF2_MAGIC) {
        s_cputs("[KERNEL] Default font is PSF 2\n\r");
    }

    //mapPage((void *)0x400000000, (void *)0x400000000, 1);
    //print_page_map(page_table_l4);

    mbiFramebuffer *mbFramebuffer = getMbiEntry(mbi, mbir.fb);
    if (mbFramebuffer->fbType == 1) {
        lockPages(mbFramebuffer->buffer, (((mbFramebuffer->pitch * mbFramebuffer->height) * mbFramebuffer->bpp) >> 12) + 1);
        mapPages(mbFramebuffer->buffer, mbFramebuffer->buffer, (((mbFramebuffer->pitch * mbFramebuffer->height) * mbFramebuffer->bpp) >> 12) + 1, 1);

        char *hello = "Hello, Framebuffer!";
        for (int i = 0; hello[i]; i++) renderChar(i * 8, 0, hello[i], 0xffffff, &termFont, mbFramebuffer);
    }

    // now we can do user stuffs
    //vgaEnableCursor();
    //setColor(0x07);

    //screenFill(0, 0, 80, 25, ' ', 0x07);

    //shell(); disable shell since we have no display yet

    while (1) asm volatile("hlt");
}

void renderChar(uint64_t x, uint64_t y, char c, uint32_t color, BITMAP_FONT *font, mbiFramebuffer *fb) {
    for (int yi = 0; yi < font->height; yi++) {
        for (int xi = 0; xi < font->height; xi++) {
            if (font->buffer[yi + (c * font->height)] >> (8 - xi) & 1 ) {
                fb_pixel(x + xi, y + yi, color, fb);
            }
        }       
    }
}