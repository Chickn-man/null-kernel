section .multiboot_header

header_start:
    dd 0xe85250d6 ; multiboot2 magic
    dd 0 ; protected mode i386
    dd header_end - header_start ; size
    dd 0x100000000 - (0xe85250d6 + (header_end - header_start)) ; check sum
    
    align 8
    ; console tag
    dw 4 ; type
    dw 0 ; flags
    dd 12 ; size
    dd 0x3 ; console_flags (ega text mode)

    ;align 8
    ; framebuffer tag
    ;dw 5 ; type
    ;dw 0 ; flags
    ;dd 20 ; size
    ;dd 0 ; prefered width (0 means no preference)
    ;dd 0 ; prefered height (0 means no preference)
    ;dd 0 ; prefered depth (0 means no preference)

    align 8
    ; end tag
	dw 0 ; type
	dw 0 ; flags
	dd 8 ; size
header_end: