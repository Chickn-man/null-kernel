/*
#  paging crap
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

#ifndef _PAGING_PAGING_H
#define _PAGING_PAGING_H

#include "../math.h"

#include <stdint.h>

typedef struct {
    uint64_t p : 1; // present
    uint64_t rw : 1; // read/write
    uint64_t us : 1; // user/super
    uint64_t pwt : 1; // write through
    uint64_t pcd : 1; // cache disable
    uint64_t a : 1; // accessed
    uint64_t avl0 : 1; // available
    uint64_t ps : 1; // page size
    uint64_t avl1 : 4; // available
    uint64_t addr : 41; // address
    uint64_t avl2 : 10; // available
    uint64_t xd : 1; // execute disable
} __attribute__((packed)) PDE_old; //stuct not used

typedef struct {
    uint64_t PDP;
    uint64_t PD;
    uint64_t PT;
    uint64_t P;
} pageMapIndex;

extern bitmap pageBitmap;

// locks page in physical memory containing paddr
// returns paddr if page was locked successfully
// returns 0 if paddr is invalid
// returns 1 if page is allready locked
// returns 2 if page is outside of physical memory
void *lockPage(void *paddr);

// locks count pages in physical memory starting at page containing paddr
// returns paddr if pages was locked successfully
// returns 0 if paddr is invalid
// returns the address + 1 of the first page that is found locked (if any are locked)
// returns the address + 2 of the first page that is found outside of physical memory (if any are outside)
void *lockPages(void *paddr, uint64_t count);

// unlocks page in physical memory containing paddr
// returns paddr if page was unlocked successfully
// returns 0 if paddr is invalid
// returns 2 if page is outside of physical memory
void *unlockPage(void *paddr);

// unlocks count pages in physical memory starting at page containing paddr
// returns paddr if pages was unlocked successfully
// returns 0 if paddr is invalid
// returns the address + 1 of the first page that is found locked (if any are locked)
// returns the address + 2 of the first page that is found outside of physical memory (if any are outside)
void *unlockPages(void *paddr, uint64_t count);

pageMapIndex getMapIndex(void *vaddr);

extern uint64_t *pageTable;
extern uint64_t page_table_l4[];

void *mapPage(void *paddr, void *vaddr, uint8_t rw);
void *mapPages(void *paddr, void *vaddr, uint64_t pages, uint8_t rw);

void *getPage();

uint64_t getPdeAddress(uint64_t pde);
uint64_t setPdeAddress(uint64_t pde, uint64_t address);

#endif // !defined _PAGING_PAGING_H