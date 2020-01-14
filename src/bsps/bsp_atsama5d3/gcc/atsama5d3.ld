/* Linker script for ATSAMA5D3 */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{

  /* 
    Real mapping :
    sram0       (rwx) : ORIGIN = 0x00300000, LENGTH = 0x00010000
  	sram1       (rwx) : ORIGIN = 0x00310000, LENGTH = 0x00010000
  */  
  sram          (rwx) : ORIGIN = 0x00300000, LENGTH = 0x00020000
  
}

SECTIONS
{
  /* Code */
  .text :
  {
    *(.startup); 
    *(.text);
  } > sram

  /* Constants */
  .rodata ALIGN(4) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
  } > sram

  /* Constructors */
  .ctors ALIGN(4) :
  {
    _CTORS_START = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    _CTORS_END = .;
  } > sram
  
  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx ALIGN(4) : 
  { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } > sram
  PROVIDE_HIDDEN (__exidx_end = .);

  /* Stack */
  .stack :
  {
    . = ALIGN(8) + 0x00000600;
    _TOP_OF_IRQ_STACK_ = .;
    . = ALIGN(8) + 0x00000600;
    _TOP_OF_SVC_STACK_ = .;
    . = ALIGN(8) + 0x00000600;
    _TOP_OF_USR_STACK_ = .;
  } > sram
  
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
  } > sram
  
  /* Uninitialized data */
  .bss :
  {
    _BSS_START_ = .;
    __bss_start__ = .;
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    _BSS_END_ = .;
    __bss_end__ = .;
  } > sram

  /* Initialized data */
  .data :
  {
    *(.data)
    . = ALIGN(4);
  } > sram
  
}

