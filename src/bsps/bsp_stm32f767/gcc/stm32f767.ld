/* Linker script for STM32F767 */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{

  rom       (rx)  : ORIGIN = 0x08000000, LENGTH = 0x00200000
  itcm_ram  (rwx) : ORIGIN = 0x00000000, LENGTH = 0x00004000
  dtcm_ram  (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00020000
  sram      (rwx) : ORIGIN = 0x20020000, LENGTH = 0x00060000
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

  /* Constructors */
  .ctors ALIGN(4) :
  {
    _CTORS_START = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    _CTORS_END = .;
  } > rom
  
  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx ALIGN(4) : 
  { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } > rom
  PROVIDE_HIDDEN (__exidx_end = .);

  /* Stack */
  .stack :
  {
    . = ALIGN(4) + 0x00000600;
    _TOP_OF_STACKS_ = .;
  } > dtcm_ram
  
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
  } > dtcm_ram
  
  /* Uninitialized data */
  .bss :
  {
    _BSS_START_ = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _BSS_END_ = .;
  } > dtcm_ram

  /* Initialized data */
  .data ALIGN(4) : AT(ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
  {
    _DATA_RAM_START_ = .;
    *(.data)
    . = ALIGN(4);
    _DATA_RAM_END_ = .;
  } > dtcm_ram
}

