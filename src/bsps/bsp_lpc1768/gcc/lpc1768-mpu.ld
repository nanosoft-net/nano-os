/* Linker script for LPC1768 with MPU */


OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")


MEMORY
{

  rom       (rx)  : ORIGIN = 0x00000000, LENGTH = 0x00080000
  ram       (rwx) : ORIGIN = 0x10000000, LENGTH = 0x00008000
}

SECTIONS
{
  /* Code */
  .text :
  {
  	_COMMON_CODE_START_ = .;
  	
    *(.startup)
    *(.text)
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


  /* Constants */
  .rodata ALIGN(4) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(4);
    _COMMON_CODE_END_ = .;
    _DATA_ROM_START_ = .;
  } > rom
  
  
  /* Nano OS internal task variables */
  .nano_os_internal_task_var : ALIGN(1024)
  {
    _NANO_OS_MODULES_RO_DATA_START_ = .;
  	_IDLE_TASK_VAR_START_ = .;
	*/nano_os_idle_task_stack.o(.bss)
	*/nano_os_idle_task_stack.o(COMMON)
	*/nano_os_idle_task_stack.o(.data)
	. = ALIGN(4);
	_IDLE_TASK_VAR_END_ = .;
	
	_ISR_REQUEST_TASK_VAR_START_ = .;
	*/nano_os_isr_request_task_stack.o(.bss)
	*/nano_os_isr_request_task_stack.o(COMMON)
	*/nano_os_isr_request_task_stack.o(.data)
	. = ALIGN(4);
	_ISR_REQUEST_TASK_VAR_END_ = .;
	
	_TIMER_TASK_VAR_START_ = .;
	*/nano_os_timer_task_stack.o(.bss)
	*/nano_os_timer_task_stack.o(COMMON)
	*/nano_os_timer_task_stack.o(.data)
	. = ALIGN(4);
	_TIMER_TASK_VAR_END_ = .;
	
  } > ram
  
  /* Console module variables */
  .console_module_var : ALIGN(2048)
  {
    _CONSOLE_MODULE_VAR_START_ = .;
	*/nano_os_console_task.o(.bss)
	*/nano_os_console_task.o(COMMON)
	*/nano_os_console_task.o(.data)
	*/libnano-os.a:nano_os_console.o(.bss)
	*/libnano-os.a:nano_os_console.o(COMMON)
	*/libnano-os.a:nano_os_console.o(.data)
	_CONSOLE_MODULE_VAR_END_ = .;
  } > ram
  
  /* Debug module variables */
  .debug_module_var : ALIGN(2048)
  {
    _DEBUG_MODULE_VAR_START_ = .;
	*/nano_os_debug_task.o(.bss)
	*/nano_os_debug_task.o(COMMON)
	*/nano_os_debug_task.o(.data)
	*/libnano-os.a:nano_os_debug.o(.bss)
	*/libnano-os.a:nano_os_debug.o(COMMON)
	*/libnano-os.a:nano_os_debug.o(.data)
	_DEBUG_MODULE_VAR_END_ = .;
  } > ram
  
  /* Main task variables */
  .main_task_var : ALIGN(2048)
  {
  	_MAIN_TASK_VAR_START_ = .;
	*/main_task.o(.bss)
	*/main_task.o(COMMON)
	*/main_task.o(.data)
	_MAIN_TASK_VAR_END_ = .;
  } > ram
  
  /* LED task variables */
  .led_task_var : ALIGN(2048)
  { 	
  	_LED_TASK_VAR_START_ = .;
	*/led_task.o(.bss)
	*/led_task.o(COMMON)
	*/led_task.o(.data)
	_LED_TASK_VAR_END_ = .;
  } > ram
  
  /* OS variables */
  .os_var : ALIGN(4096)
  { 	
  	_OS_VAR_START_ = .;
	*/libnano-os.a:*(.bss)
	*/libnano-os.a:*(COMMON)
	*/libnano-os.a:*(.data)
  } > ram
  
  /* Stack */
  .stack :
  {
    . = ALIGN(4) + 0x00000600;
    _TOP_OF_STACKS_ = .;
    _OS_VAR_END_ = .;
    _NANO_OS_MODULES_RO_DATA_END_ = .;
  } > ram
  
  /* Heap */
  .heap : ALIGN(4096)
  {
  	_COMMON_DATA_START_ = .;
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
  .data ALIGN(4) : AT(ADDR(.rodata) + SIZEOF(.rodata))
  {
    _DATA_RAM_START_ = .;
    *(.data)
    . = ALIGN(4);
    _DATA_RAM_END_ = .;
    _COMMON_DATA_END_ = .;
  } > ram
  
  

}

