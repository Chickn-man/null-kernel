#
#  Makefile for building the null Kernel
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

PROGNAME = null

VERSION_REL = 0
VERSION_MAJ = 1
VERSION_MIN = 0
VERSION_FIX = 0

CC = gcc
AS = nasm
LD = ld

INCS =
LIBS = 

LDS = -T kernel.ld
CFLAGS = $(LIBS) $(INCS) -O1 -nostdlib -ffreestanding -fno-stack-protector -DVERSION_REL=$(VERSION_REL) -DVERSION_MAJ=$(VERSION_MAJ) -DVERSION_MIN=$(VERSION_MIN) -DVERSION_FIX=$(VERSION_FIX)
ASFLAGS = -f elf64
LDFLAGS = -static -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR := bin
IMAGEDIR := iso

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)
SSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(SSRC))
DIRS = $(wildcard $(SRCDIR)/*)

.PHONY: build
build: setup link image

.PHONY: link
link: $(OBJS)
	@ echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$(PROGNAME)-kernel $(LDS) $(OBJS) 

$(OBJDIR)/interrupts/handlers.o: $(SRCDIR)/interrupts/handlers.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -mgeneral-regs-only -c $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@ echo !==== ASSEMBLING $^
	@ mkdir -p $(@D)
	$(AS) $(ASFLAGS) $^ -o $@

.PHONY: image
image:
	@ cp $(BUILDDIR)/$(PROGNAME)-kernel $(IMAGEDIR)
	@ grub-mkrescue /usr/lib/grub/i386-pc -o $(BUILDDIR)/$(PROGNAME).iso $(IMAGEDIR)

.PHONY: setup
setup:
	@ mkdir -p $(SRCDIR)
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	-@ rm -r $(BUILDDIR)
	-@ rm -r $(OBJDIR)

.PHONY: run
run:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown

run:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -accel kvm -m 4g

run-debug:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -d int -M smm=off