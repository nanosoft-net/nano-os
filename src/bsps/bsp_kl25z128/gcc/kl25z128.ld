/* Linker script for KL25Z128 */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{
  rom_interrupts (rx) : ORIGIN = 0x00000000, LENGTH = 0x00000400
  rom_cfmprotect (rx) : ORIGIN = 0x00000400, LENGTH = 0x00000010
  rom_app        (rx) : ORIGIN = 0x00000410, LENGTH = 0x0001FBF0
  ram            (rwx) : ORIGIN = 0x1FFFF000, LENGTH = 0x00004000
  
  /* In fact embedded SRAM is split into 2 contiguous areas as follow : 
  sram_l    (rwx) : ORIGIN = 0x1FFFF000, LENGTH = 0x00001000
  sram_u    (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00003000
  */
}

SECTIONS
{

  /* Interrupt vector table */
  .interrupts :
  {
    *(.text.vector_table);
    . = ALIGN(4);
    FILL(0xDEADC0DE);
  } > rom_interrupts
  
  /* Flash memory protection */
  .cfmprotect :
  {
    *(.text.cfm_protect);
    . = ALIGN(4);
    FILL(0xDEADC0DE);
  } > rom_cfmprotect
  
  /* Code */
  .text :
  {
    /* Explicit placement needed because long jmp is not supported on Cortex-M0+ arch
       so interrupt handlers must be close to interrupt vector table */
	*(.nano_os_port_asm)
	*(.startup)   
    *(.text)
  } > rom_app


  /* Constants */
  .rodata ALIGN(4) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _DATA_ROM_START_ = .;
  } > rom_app

  /* Constructors */
  .ctors ALIGN(4) :
  {
    _CTORS_START = .;
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array))
    _CTORS_END = .;
  } > rom_app
  
  PROVIDE_HIDDEN (__exidx_start = .);
  .ARM.exidx ALIGN(4) : 
  { 
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
  } > rom_app
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
    . = ALIGN(4) + 0x0800;
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

