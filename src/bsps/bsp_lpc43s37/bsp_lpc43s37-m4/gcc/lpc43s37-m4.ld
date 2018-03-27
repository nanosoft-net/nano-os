/* Linker script for LPC43S37-M4 */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{
  rom_m4    (rx)  : ORIGIN = 0x1A000000, LENGTH = 0x00080000
  rom_m0    (rx)  : ORIGIN = 0x1B000000, LENGTH = 0x00080000
  ram_m4    (rwx) : ORIGIN = 0x10000000, LENGTH = 0x00012000
  ram_m0    (rwx) : ORIGIN = 0x20000000, LENGTH = 0x0000C000
  ram_ipc   (rwx) : ORIGIN = 0x2000C000, LENGTH = 0x00004000
}

SECTIONS
{
  /* Code */
  .text :
  {
    *(.startup)
    *(.text)
  } > rom_m4


  /* Constants */
  .rodata ALIGN(4) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _DATA_ROM_START_ = .;
  } > rom_m4

  /* Constructors */
  .ctors ALIGN(4) :
  {
    _CTORS_START = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    _CTORS_END = .;
  } > rom_m4


  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx ALIGN(4) : 
  { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } > rom_m4
  PROVIDE_HIDDEN (__exidx_end = .);

  /* Stack */
  .stack :
  {
    . = ALIGN(4) + 0x00000600;
    _TOP_OF_STACKS_ = .;
  } > ram_m4
  
  /* Heap */
  .heap :
  {
    . = ALIGN(4);
    __end__ = .;
    end = __end__;
    __HeapBase = __end__;
    *(.heap*)
    . = ALIGN(4) + 0x0800;
    __HeapLimit = .;
  } > ram_m4
  
  /* Uninitialized data */
  .bss :
  {
    _BSS_START_ = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _BSS_END_ = .;
  } > ram_m4

  /* Initialized data */
  .data ALIGN(4) : AT(ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
  {
    _DATA_RAM_START_ = .;
    *(.data)
    . = ALIGN(4);
    _DATA_RAM_END_ = .;
  } > ram_m4
  
  

}

