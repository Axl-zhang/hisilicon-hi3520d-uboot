OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 . = ALIGN(4);
 .text :
 {
  __text_start = .;
  arch/arm/cpu/hi3521a/start.o (.text)
  drivers/ddr/ddr_training_impl.o (.text)
  drivers/ddr/ddr_training_ctl.o (.text)
  drivers/ddr/ddr_training_boot.o (.text)
  drivers/ddr/ddr_training_custom.o (.text)
  __init_end = .;
  ASSERT(((__init_end - __text_start) < 0x3B00), "init sections too big!");
  *(.text)
 }
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
 . = ALIGN(4);
 .data : { *(.data) }
 . = ALIGN(4);
 .got : { *(.got) }
 __u_boot_cmd_start = .;
 .u_boot_cmd : { *(.u_boot_cmd) }
 __u_boot_cmd_end = .;
 . = ALIGN(4);
 __bss_start = .;
 .bss : { *(.bss) }
 _end = .;
}
