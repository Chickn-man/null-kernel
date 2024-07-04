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
extern main, mbi

section .text

[bits 32]

_start:
	; store multiboot information
	mov [mbi], ebx

    ; house keeping
    mov esp, stack_top
    
    call check_multiboot

    mov esi, s_hello
    call print

    call check_cpuid
	call check_long_mode
	call setup_page_tables
	call enable_paging
	lgdt [gdt.pointer]

%ifdef DEBUG
	mov esi, s_jump_to_long
	call print
%endif

	jmp gdt.code:long_start

halt:
    hlt
    jmp halt

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
	mov ecx, 0 ; counter
.loop:
	mov eax, 0x400000 ; 4MiB
	mul ecx
	or eax, 0b10000011 ; present, writable, huge page
	mov [page_table_l2 + ecx * 8], eax
	inc ecx ; increment counter
	cmp ecx, 512 ; checks if the whole table is mapped
	jne .loop ; if not, continue
	ret

enable_paging:
	; pass page table location to cpu
	mov eax, page_table_l4
	mov cr3, eax
	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
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

print: ; puts string in esi on the screen 
    mov edi, 0xb8000
.loop: 
    lodsb
    or al, al
    jz .exit
    mov ah, 0x07 ; grey on black
    mov [edi], ax
    adc edi, 2
    jmp .loop
.exit:
    ret

section .data

s_hello: db "Booting...", 0
s_no_multiboot: db "Error: not booted with multiboot", 0
s_no_cpuid: db "Error: no cpuid", 0
s_no_long_mode: db "Error: CPU is not 64 bit", 0

; DEBUG meessages
%ifdef DEBUG
s_jump_to_long: db "Entering 64-bit code", 0
%endif

section .bss

align 4096
page_table_l4:
	resb 4096
page_table_l3:
	resb 4096
page_table_l2:
	resb 4096
stack_bottom:
	resb 4096 * 4
stack_top:

section .rodata

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