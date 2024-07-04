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

#include "paging.h"
#include "../math.h"
#include "../memory.h"
#include "../conio.h"
#include "../string.h"

bitmap pageBitmap;

// locks page in physical memory containing paddr
// returns paddr if page was locked successfully
// returns 0 if paddr is invalid
// returns 1 if page is allready locked
// returns 2 if page is outside of physical memory
void *lockPage(void *paddr) {
    uint64_t address = (uint64_t)paddr & 0xfffffffffffff000; 
    if (bitmapGet(address >> 12, &pageBitmap)) return (void*)1;
    // TODO check if page is outside of physical memory
    bitmapSet(address >> 12, &pageBitmap, 1);
    return paddr;
}

// locks count pages in physical memory starting at page containing paddr
// returns paddr if pages was locked successfully
// returns 0 if paddr is invalid
// returns the address + 1 of the first page that is found locked (if any are locked)
// returns the address + 2 of the first page that is found outside of physical memory (if any are outside)
void *lockPages(void *paddr, uint64_t count) {
    uint64_t address = (uint64_t)paddr & 0xfffffffffffff000; 

    for (int i = 0; i < count; i++) {
        if (bitmapGet((address >> 12) + i, &pageBitmap)) return (void*)((((address >> 12) + i) << 12) + 1);
        // TODO check if page is outside of physical memory
        bitmapSet((address >> 12) + i, &pageBitmap, 1);
    }

    return paddr;
}

// unlocks page in physical memory containing paddr
// returns paddr if page was unlocked successfully
// returns 0 if paddr is invalid
// returns 2 if page is outside of physical memory
void *unlockPage(void *paddr) {
    uint64_t address = (uint64_t)paddr & 0xfffffffffffff000; 
    // TODO check if page is outside of physical memory
    bitmapSet(address >> 12, &pageBitmap, 0);
    return paddr;
}

// unlocks count pages in physical memory starting at page containing paddr
// returns paddr if pages was unlocked successfully
// returns 0 if paddr is invalid
// returns the address + 2 of the first page that is found outside of physical memory (if any are outside)
void *unlockPages(void *paddr, uint64_t count) {
    uint64_t address = (uint64_t)paddr & 0xfffffffffffff000; 

    for (int i = 0; i < count; i++) {
        // TODO check if page is outside of physical memory
        bitmapSet((address >> 12) + i, &pageBitmap, 0);
    }

    return paddr;
}

pageMapIndex getMapIndex(void *vaddr) {
    uint64_t address = (uint64_t)vaddr;
    address &= 0xfffffffffffff000;
    address >>= 12;
    pageMapIndex ret;
    ret.P = address & 0x1ff;
    address >>= 9;
    ret.PT = address & 0x1ff;
    address >>= 9;
    ret.PD = address & 0x1ff;
    address >>= 9;
    ret.PDP = address & 0x1ff;
}

PDE *pageTable = page_table_l4; // this shit fixed everything
//uint8_t thingy = 0; // past me: this is stupid | preset me: what does this do???

void *mapPage(void *paddr, void *vaddr, uint8_t rw) { // basically poncho's code | broken
    pageMapIndex index = getMapIndex(vaddr);

    PDE pde = pageTable[index.PDP];
    PDE *pdp; // array of 512 entries
    if (!pde.p) {
        pdp = getPage();
        mapPage(pdp, pdp, 1);
        if (!pdp) return 0;
        memset(pdp, 0, 0x1000);
        pde.addr = (uint64_t)pdp >> 12;
        pde.p = 1;
        pde.rw = 1;
        pageTable[index.PDP] = pde;
    } else {
        pdp = (PDE *)(pde.addr << 12);
    }

    pde = pdp[index.PD];
    PDE *pd; // array of 512 entries
    if (!pde.p) {
        pd = getPage();
        mapPage(pd, pd, 1);
        if (!pd) return 0;
        memset(pd, 0, 0x1000);
        pde.addr = (uint64_t)pd >> 12;
        pde.p = 1;
        pde.rw = 1;
        pdp[index.PD] = pde;
    } else {
        pd = (PDE *)(pde.addr << 12);
    }

    pde = pd[index.PT];
    PDE *pt; // array of 512 entries
    if (!pde.p) {
        pt = getPage();
        mapPage(pt, pt, 1);
        if (!pt) return 0;
        memset(pt, 0, 0x1000);
        pde.addr = (uint64_t)pt >> 12;
        pde.p = 1;
        pde.rw = 1;
        pd[index.PT] = pde;
    } else {
        pt = (PDE *)(pde.addr << 12);
    }

    pde = pt[index.P];
    pde.addr = (uint64_t)paddr >> 12;
    pde.p = 1;
    pde.rw = rw;
    pt[index.P] = pde;

    char buffalo[64];

    /*cputs("#! mapped 0x");
    cputs(itoa((long int)paddr, buffalo, 16));
    cputs(" to 0x");
    cputs(itoa((long int)vaddr, buffalo, 16));
    cputs(" !#\n\r");*/
}

void *mapPages(void *paddr, void *vaddr, uint64_t pages, uint8_t rw) {
    for (uint64_t i = 0; i < pages; i++) {
        mapPage((void *)((uint64_t)paddr + (i << 12)), (void *)((uint64_t)vaddr + (i << 12)), rw);
    }
}

void *getPage() {
    for (uint64_t i = 0; i < pageBitmap.size; i++) {
        if (!bitmapGet(i, &pageBitmap)) {
            lockPage((void *)(i << 12));
            return (void *)(i << 12);
        }
    }
    
    return 0;
}