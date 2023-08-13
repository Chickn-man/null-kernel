/*
#  keyboard stuffs for the null Kernel
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

#ifndef _IO_HID_KEYBOARD_H
#define _IO_HID_KEYBOARD_H

#include <stdint.h>

extern char usKeyLower[];

extern char usKeyUpper[];

#define usKeyLeftShift 0x2A
#define usKeyRightShift 0x36
#define usKeyEnter 0x1C
#define usKeyBackSpace 0x0E
#define usKeySpacebar 0x39

#define usKeyUp 0x48
#define usKeyDown 0x50
#define usKeyLeft 0x4B
#define usKeyRight 0x4D

extern char keyBuffer[65];

#endif // !defined _IO_HID_KEYBOARD_H