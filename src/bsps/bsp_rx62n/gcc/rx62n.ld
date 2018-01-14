/* Linker script for RX62N */


OUTPUT_ARCH ("rx")


MEMORY
{

  rom                 (rx)  : ORIGIN = 0xFFF80000, LENGTH = 0x00080000 /*0x0007FF80*/
  /*fixed_vector_table  (rx)  : ORIGIN = 0xFFFFFF80, LENGTH = 0x00000080*/
  sram                (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00018000
}

SECTIONS
{

  /* Code */
  .text :
  {
    *(.startup)
    *(.text)
    
    
  } > rom


  /* Constants */
  .rodata ALIGN(4) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _DATA_ROM_START_ = .;
  } > rom
  
  /* Fixed vector table */
  .fvtable :
  {
    . = (ALIGN(0x80000) - 0x80);
    /* Fixed vector table */
    *(.fixed_vector_table)
  } > rom
  
  /* Stack */
  .stack :
  {
    . = ALIGN(4) + 0x00000600;
    _TOP_OF_USTACK_ = .;
    . = ALIGN(4) + 0x00000600;
    _TOP_OF_ISTACK_ = .;
  } > sram
  
  /* Heap */
  .heap :
  {
    . = ALIGN(4);
    __end__ = .;
    end = __end__;
    __HeapBase = __end__;
    *(.heap*)
    . = ALIGN(4) + 0x2000;
    __HeapLimit = .;
  } > sram
    
  /* Uninitialized data */
  .bss :
  {
    _BSS_START_ = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _BSS_END_ = .;
  } > sram

  /* Initialized data */
  .data ALIGN(4) : AT(ADDR(.rodata) + SIZEOF(.rodata))
  {
    _DATA_RAM_START_ = .;
    *(.data)
    . = ALIGN(4);
    _DATA_RAM_END_ = .;
  } > sram
}

