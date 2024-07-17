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
VERSION_MIN = 3
VERSION_FIX = 2

CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy

INCS =
LIBS = 

LDS = -T kernel.ld
override CFLAGS := $(LIBS) $(INCS) -g -O0 -nostdlib -ffreestanding -fno-stack-protector -DVERSION_REL=$(VERSION_REL) -DVERSION_MAJ=$(VERSION_MAJ) -DVERSION_MIN=$(VERSION_MIN) -DVERSION_FIX=$(VERSION_FIX) $(CFLAGS)
override ASFLAGS := -f elf64 -g -F dwarf $(ASFLAGS)
override LDFLAGS := -static -nostdlib $(LDFLAGS)

FONTDIR := fonts
SRCDIR := src
OBJDIR := lib
BUILDDIR := bin
IMAGEDIR := iso

EFIBIOS := /usr/share/ovmf/x64/OVMF_CODE.fd

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.c)
SSRC = $(call rwildcard,$(SRCDIR),*.asm)
FSRC = $(call rwildcard,$(FONTDIR),*.psf)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(SSRC))
OBJS += $(patsubst $(FONTDIR)/%.psf, $(OBJDIR)/fonts/%.o, $(FSRC))
DIRS = $(wildcard $(SRCDIR)/*)

ifeq ("$(wildcard $(FONTDIR)/default.psf)","")
$(shell mkdir -p $(FONTDIR))
$(error No default font found! Please place a psf font at $(FONTDIR)/default.psf)
endif

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

$(OBJDIR)/fonts/%.o: $(FONTDIR)/%.psf
	@ echo !==== CONVERTING $^
	@ mkdir -p $(@D)
	$(OBJCOPY) -O elf64-x86-64 -B i386 -I binary $^ $@

.PHONY: image
image:
	@ cp $(BUILDDIR)/$(PROGNAME)-kernel $(IMAGEDIR)
	@ grub-mkrescue /usr/lib/grub/i386-pc -o $(BUILDDIR)/$(PROGNAME).iso $(IMAGEDIR)

.PHONY: setup
setup:
	@ mkdir -p $(FONTDIR)
	@ mkdir -p $(SRCDIR)
	@ mkdir -p $(OBJDIR)
	@ mkdir -p $(BUILDDIR)

.PHONY: clean
clean:
	-@ rm -r $(BUILDDIR)
	-@ rm -r $(OBJDIR)

.PHONY: run
run:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -m 4g -serial stdio

.PHONY: run-kvm
run-kvm:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -accel kvm -m 4g -serial stdio

.PHONY: run-hvf
run-hvf:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -accel hvf -m 4g -serial stdio

.PHONY: run-debug
run-debug:
	qemu-system-x86_64 -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -m 4g -d int -M smm=off -s -S

.PHONY: run-efi
run-efi:
	qemu-system-x86_64 -bios $(EFIBIOS) -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -m 4g -serial stdio

.PHONY: run-efi-kvm
run-efi-kvm:
	qemu-system-x86_64 -bios $(EFIBIOS) -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -accel kvm -m 4g -serial stdio

.PHONY: run-efi-hvf
run-efi-hvf:
	qemu-system-x86_64 -bios $(EFIBIOS) -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -accel hvf -m 4g -serial stdio

.PHONY: run-efi-debug
run-efi-debug:
	qemu-system-x86_64 -bios $(EFIBIOS) -drive format=raw,file=$(BUILDDIR)/$(PROGNAME).iso -no-reboot -no-shutdown -m 4g -d int -M smm=off -s -S