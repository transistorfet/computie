ENTRY(_start)
EXTERN(_start)
SECTIONS
{
  .text 0x100000 : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
