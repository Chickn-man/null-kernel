/*
#  Multiboot2 Information Parsing stuffs
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

#ifndef _MULTIBOOT_MBI_H
#define _MULTIBOOT_MBI_H

#include <stdint.h>

typedef struct {
    uint32_t size;
    uint32_t reserved;
} __attribute__((packed)) mbiFixed;

typedef struct {
    uint32_t type;
    uint32_t size;
} __attribute__((packed)) mbiGeneric;

typedef struct {
    uint32_t type; // 4
    uint32_t size; // 16
    uint32_t memLower;
    uint32_t memUpper;
} __attribute__((packed)) mbiMemory;

typedef struct {
    uint32_t type; // 5
    uint32_t size; // 20
    uint32_t dev;
    uint32_t part;
    uint32_t subPart;
} __attribute__((packed)) mbiBiosBoot;

typedef struct {
    uint32_t type; // 1
    uint32_t size;
    char string; // (&string)[i]
} __attribute__((packed)) mbiCmdLine;

typedef struct {
    uint32_t type; // 3
    uint32_t size;
    uint32_t start;
    uint32_t end;
    char string; // (&string)[i]
} __attribute__((packed)) mbiModule;

typedef struct {
    uint32_t type; // 9
    uint32_t size;
    uint16_t num;
    uint16_t entsize;
    uint16_t shndx;
    uint16_t reserved;
    int headers; // not sure what type this should be (prolly an array of elf section headers (no elf structs yet))
} __attribute__((packed)) mbiElf;

typedef struct {
    uint8_t reserved;
    uint8_t free;
    uint8_t acpi;
    uint8_t hiber;
    uint8_t defct;
} MEMMAP;

extern const MEMMAP memMap;

typedef struct {
    uint64_t addr;
    uint64_t size;
    uint32_t type;
    uint32_t reserved;
} __attribute__((packed)) mapEntry;

typedef struct {
    uint32_t type; // 6
    uint32_t size;
    uint32_t entrySize;
    uint32_t version;
    uint64_t entries; // (*((mapEntry*)(&entries + (entrySize * i))))
} __attribute__((packed)) mbiMap;

typedef struct {
    uint32_t type; // 2
    uint32_t size;
    char string; // (&string)[i]
} __attribute__((packed)) mbiLoaderName;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) RGB;

typedef struct {
    uint32_t colors;
    RGB palette;
} __attribute__((packed)) fbPalette;

typedef struct {
    uint8_t redPos;
    uint8_t redMask;
    uint8_t greenPos;
    uint8_t greenMask;
    uint8_t bluePos;
    uint8_t blueMask;
} __attribute__((packed)) fbDirect;

typedef struct {
    uint32_t type; // 7
    uint32_t size; // 784
    uint16_t mode;
    uint16_t segment;
    uint16_t address;
    uint16_t length;
    uint8_t controlinfo[512];
    uint8_t modeInfo[256];
} __attribute__((packed)) mbiVbe;

typedef struct {
    uint32_t type; // 8
    uint32_t size;
    uint32_t *buffer;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t fbType;
    uint8_t reserved;
    fbDirect colorInfo;
} __attribute__((packed)) mbiFramebuffer;

typedef struct {
    uint8_t cmdLine;
    uint8_t loaderName;
    uint8_t module;
    uint8_t memory;
    uint8_t bootDev;
    uint8_t memoryMap;
    uint8_t vbe;
    uint8_t fb;
    uint8_t elf;
} MBIR;

extern const MBIR mbir;

// returns a pointer to the nth accurance of entry type "type"
// returns null if entry of type "type" was not found
void *getMbiEntry(void *mbi, uint32_t type);

#endif