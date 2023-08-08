ENTRY(_start)
EXTERN(_start)
SECTIONS
{
  .vectors 0x0000 : { *(.vectors) }
  .text 0x0040 : { *(.text) }
  .rodata : { *(.rodata) }
  /*. = 0x1400;*/
  .data : { *(.data) }
  .bss : { *(.bss) }
}
