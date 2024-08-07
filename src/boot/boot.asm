;#
;#  long mode setup stuffs
;#
;##############################################################################
;#
;#  Copyright (C) 2023 Keegan Powers
;#
;#  This file is part of the null Kernel
;#
;#  The null Kernel is free software: you can redistribute it
;#  and/or modify it under the terms of the GNU General Public
;#  License as published by the Free Software Foundation, either
;#  version 3 of the License, or (at your option) any later version.
;#
;#  This program is distributed in the hope that it will be useful,
;#  but WITHOUT ANY WARRANTY; without even the implied warranty of
;#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;#  GNU General Public License for more details.
;#
;#  You should have received a copy of the GNU General Public License
;#  along with this program. If not, see <https://www.gnu.org/licenses/>.
;#
;##############################################################################

global _start
global page_table_l4
extern main, mbi

%macro outb 2
    mov dx, %1
    mov al, %2
    out dx, al
%endmacro

%macro inb 1
    mov dx, %1
    in al, dx
%endmacro

section .text

[bits 32]

_start:
	; store multiboot information
	mov [mbi], ebx

    ; house keeping
    mov esp, stack_top
    
    call check_multiboot

	call init_serial

    mov esi, s_hello
    call print

    call check_cpuid
	call check_long_mode
	call setup_page_tables
	call enable_paging
	lgdt [gdt.pointer]

	jmp gdt.code:long_start

halt:
    hlt
    jmp halt

init_serial:
	outb COM1 + 1, 0x00;    // Disable all interrupts
   	outb COM1 + 3, 0x80;    // Enable DLAB (set baud rate divisor)
   	outb COM1 + 0, 0x0C;    // Set divisor to 12 (lo byte) 9600 baud
   	outb COM1 + 1, 0x00;    //                   (hi byte)
   	outb COM1 + 3, 0x03;    // 8 bits, no parity, one stop bit
   	outb COM1 + 2, 0xC7;    // Enable FIFO, clear them, with 14-byte threshold
	outb COM1 + 4, 0x0B;    // Normal
	ret

check_multiboot:
	cmp eax, 0x36d76289
	jne .no_multiboot
	ret
.no_multiboot:
	mov esi, s_no_multiboot
	call print
    jmp halt

check_cpuid:
	pushfd
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	cmp eax, ecx
	je .no_cpuid
	ret
.no_cpuid:
	mov esp, s_no_cpuid
    call print
    jmp halt

check_long_mode:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .no_long_mode
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .no_long_mode
	ret
.no_long_mode:
	mov esi, s_no_long_mode
	call print
	jmp halt

setup_page_tables:
	mov eax, page_table_l3
	or eax, 0b11 ; present, writable
	mov [page_table_l4], eax

	mov eax, page_table_l2
	or eax, 0b11 ; present, writable
	mov [page_table_l3], eax

	mov eax, page_table_l1
	or eax, 0b11 ; present, writable
	mov [page_table_l2], eax

	mov eax, page_table_l1 + 0x1000
	or eax, 0b11 ; present, writable
	mov [page_table_l2 + 8], eax

	mov ecx, 0 ; counter
.loop: ; this maps 4MiB using 4KiB pages, it creates 512 entries in two page tables
	mov eax, 0x1000 ; 4KiB
	mul ecx
	or eax, 0b00000011 ; present, writable
	mov [page_table_l1 + ecx * 8], eax
	inc ecx ; increment counter
	cmp ecx, 1024 ; checks if both tables are mapped
	jne .loop ; if not, continue
	ret

enable_paging:
	; pass page table location to cpu
	mov eax, page_table_l4
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
	and eax, ~(1 << 12)
	mov cr4, eax

	; enable long mode
	mov ecx, 0xC0000080
	rdmsr
	or eax, 1 << 8
	wrmsr

	; enable paging
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax
	ret

print: ; puts string in esi on the serial port
.loop: 
    lodsb ; move char at [esi] into al and increment esi
    or al, al ; end of string?
    jz .exit
    outb COM1, al
    jmp .loop
.exit:
    ret

section .data

s_hello: db `[BOOT] Booting...\n\r`, 0
s_no_multiboot: db `[BOOT] Error: not booted with multiboot\n\r`, 0
s_no_cpuid: db `[BOOT] Error: no cpuid\n\r`, 0
s_no_long_mode: db `[BOOT] Error: CPU is not 64 bit\n\r`, 0
section .bss

align 4096
page_table_l4:
	resb 4096
page_table_l3:
	resb 4096
page_table_l2:
	resb 4096
page_table_l1:
	resb 4096 * 2
stack_bottom:
	resb 4096 * 4
stack_top:

section .rodata

COM1 equ 0x3f8

; Access bits
PRESENT equ 1 << 7
NOT_SYS equ 1 << 4
EXEC equ 1 << 3
DC equ 1 << 2
RW equ 1 << 1
ACCESSED equ 1 << 0
 
; Flags bits
GRAN_4K equ 1 << 7
SZ_32 equ 1 << 6
LONG_MODE equ 1 << 5

gdt:
.null: equ $ - gdt
	dq 0 ; zero entry
.code: equ $ - gdt
	dd 0xffff
	db 0
	db PRESENT | NOT_SYS | EXEC | RW
	db GRAN_4K | LONG_MODE | 0xf
	db 0
.data: equ $ - gdt
	dd 0xffff
	db 0
	db PRESENT | NOT_SYS | RW
	db GRAN_4K | SZ_32 | 0xf
	db 0
.tss: equ $ - gdt
	dd 0x00000068
	dd 0x00CF8900
.pointer:
	dw $ - gdt - 1 ; length
	dq gdt ; address

[bits 64]

section .text

long_start:
	cli
	mov ax, 0
  	mov ss, ax
  	mov ds, ax
  	mov es, ax
 	mov fs, ax
  	mov gs, ax
  	call main
	jmp halt