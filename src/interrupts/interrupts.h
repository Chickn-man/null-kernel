#ifndef _INTERUPTS_INTERUPTS_H
#define _INTERUPTS_INTERUPTS_H

#include <stdint.h>

#define INTR 0b10001110
#define CALL 0b10001100
#define TRAP 0b10001111
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

typedef struct {
  uint16_t offset0;
  uint16_t selector;
  uint8_t ist;
  uint8_t type_attr;
  uint16_t offset1;
  uint32_t offset2;
  uint32_t ignore;
} __attribute__((packed)) IDTEntry;

typedef struct {
  uint16_t limit;
  uint64_t offset;
} __attribute__((packed)) IDTR;

void setIdtOffset(IDTEntry *entry, uint64_t offset);
uint64_t getIdtOffset(IDTEntry *entry);

extern IDTR idtr;
void setIdtGate(void *handler, uint8_t offset, uint8_t type_attr, uint8_t selector);


#endif // !defined _INTERUPTS_INTERUPTS_H