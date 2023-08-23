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

#include "mbi.h"

const MBIR mbir = {1, 2, 3, 4, 5, 6, 7, 8, 9};
const MEMMAP memMap = {0, 1 ,3, 4, 5};

// returns a pointer to the nth accurance of entry type "type"
// returns null if entry of type "type" was not found
void *getMbiEntry(void *mbi, uint32_t type) {
    for (int i = 8; i < ((mbiFixed*)mbi)->size;) {
        if (((mbiGeneric*)(mbi + i))->type == type) return (mbi + i);

        i += (((mbiGeneric*)(mbi + i))->size + 0x7) & 0xfffffff8; // add the size to i after aligning it on an 8 byte boundary
    }

    return 0;
}