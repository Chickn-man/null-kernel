OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
SECTIONS {
  . = 1M;
  kernelStart = .;

  .boot : {
    KEEP(*(.multiboot_header))
  }

  .text : {
    *(.text)
  }

  .data : {
    *(.data)
  }

  .bss : {
    *(.bss)
  }
  
  kernelEnd = .;
}