/* Linker script for LPC11C24 */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{

  rom       (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00008000
  ram       (rwx) : ORIGIN = 0x100000BC, LENGTH = 0x00001F44
}

SECTIONS
{
  /* Code */
  .text :
  {
      /* Explicit placement needed because long jmp is not supported on Cortex-M0 arch
       so interrupt handlers must be close to interrupt vector table */
    *(.startup)
    *(.nano_os_port_asm)
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
  } > ram
  
  /* Heap */
  .heap :
  {
    . = ALIGN(4);
    __end__ = .;
    end = __end__;
    __HeapBase = __end__;
    *(.heap*)
    . = ALIGN(4) + 0x0500;
    __HeapLimit = .;
  } > ram
  
  /* Uninitialized data */
  .bss :
  {
    _BSS_START_ = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _BSS_END_ = .;
  } > ram

  /* Initialized data */
  .data ALIGN(4) : AT(ADDR(.ARM.exidx) + SIZEOF(.ARM.exidx))
  {
    _DATA_RAM_START_ = .;
    *(.data)
    . = ALIGN(4);
    _DATA_RAM_END_ = .;
  } > ram
  
  

}

