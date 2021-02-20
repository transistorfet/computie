/* OUTPUT_FORMAT(binary) */
ENTRY(_start)
EXTERN(_start)
SECTIONS
{
  /*
  .vectors 0x0000 : { *(.vectors) }
  .text 0x0020 : { *(.text) }
  */
  .text 0x100000 : { *(.text) }
  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { *(.bss) }
}
