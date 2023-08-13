/*
#  interrupt function for the null Kernel
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

#include "interrupts.h"

#include <stdint.h>

void setIdtOffset(IDTEntry* entry, uint64_t offset) {
    entry->offset0 = (uint16_t)(offset & 0x000000000000ffff);
    entry->offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    entry->offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t getIdtOffset(IDTEntry* entry) {
    uint64_t offset = 0;
    offset |= (uint64_t)entry->offset0;
    offset |= (uint64_t)entry->offset1 << 16;
    offset |= (uint64_t)entry->offset2 << 32;
    return offset;
}

IDTR idtr;
void setIdtGate(void* handler, uint8_t offset, uint8_t type_attr, uint8_t selector) {
  IDTEntry* interrupt = (IDTEntry*)(idtr.offset + (offset * sizeof(IDTEntry)));
  setIdtOffset(interrupt, (uint64_t)handler);
  interrupt->type_attr = type_attr;
  interrupt->selector = selector;
} 