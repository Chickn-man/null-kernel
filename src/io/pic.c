/*
#  pic io functions for the null Kernel
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


#include "pic.h"

void outb(uint16_t port, uint8_t byte) {
    asm volatile ("outb %0, %1" : : "a" (byte), "Nd" (port));
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void outw(uint16_t port, uint16_t word) {
    asm volatile ("outw %0, %1" : : "a" (word), "Nd" (port));
}

uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}

void outl(uint16_t port, uint32_t dword) {
    asm volatile ("outl %0, %1" : : "a" (dword), "Nd" (port));
}

uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a" (ret) : "Nd" (port));
    return ret;
}


void picWait() {
    asm volatile ("outb %%al, $0x80" : : "a" (0));
}