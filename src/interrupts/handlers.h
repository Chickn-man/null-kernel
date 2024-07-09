/*
#  interrupt handlers for the null Kernel
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

#include <stdint.h>
#include <stddef.h>

#ifndef _INTERRUPTS_HANDLERS_H
#define _INTERRUPTS_HANDLERS_H

struct interruptFrame;
__attribute__((interrupt)) void pageFaultHandler(struct interruptFrame* frame, uint64_t error);
__attribute__((interrupt)) void doubleFaultHandler(struct interruptFrame* frame);
__attribute__((interrupt)) void genProcFaultHandler(struct interruptFrame* frame);
__attribute__((interrupt)) void keyboardHandler(struct interruptFrame* frame);

#endif // !defined _INTERUPTS_HANDLERS_H