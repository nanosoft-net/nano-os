# nano-os
Nano OS is an open source preemptive real time multitasking scheduler targeting 8 to 32 bits microcontrollers.

Its main characteristics are:

* Maximum of 65535 tasks and synchronization objects
* Maximum of 255 levels of priority
* Round-robin scheduling for tasks with the same priority level
* Support of the following synchronization objects:
 * Semaphore
 * Mutex
 * Condition variable
 * Flag set
 * Mailbox
 * Waitable timer
* Highly interruptible code
* Highly portable code (95% is written in C language)
* C API is C++ compatible, a C++ API is also available
* A unique global variable containing all the internal data
