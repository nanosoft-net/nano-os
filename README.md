# Nano OS
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

## Supported architectures

Nano OS has been ported to the following architectures:

* ARM Cortex-M0
* ARM Cortex-M0+
* ARM Cortex-M0+ with MPU
* ARM Cortex-M3
* ARM Cortex-M3 with MPU
* ARM Cortex-M4 with FPU
* ARM Cortex-M4 with FPU and MPU
* ARM Cortex-M7 with FPU
* ARM Cortex-M7 with FPU and MPU
* Windows 7 and 10 32/64bits

## Building Nano OS

Nano OS source code release uses Nano Build as build system.
Nano Build is a makefile based build system which simplifies cross platform / cross target compilation.
The generated binaries files are in ELF format and are ready to use with a debugger (example : J-link + GdbServer).
If you need to get ihex files or raw binary files you can use the GNU objcopy utility to convert Nano Build output file to the wanted format.
Nano OS demo applications and Nano OS source code can also be built using custom makefile or custom IDE project.

### Building using Nano Build

#### Pre-requisites

The GNU Make 4.1 or greater utility must be installed and must be on the operating system PATH variable.
If using the python build script, Python 2.7 must be installed.

#### Configuring Nano Build

The path to the compilers must be configured in the build/make/compiler directory.
Each compiler files refers to a single compiler and must be written using the Makefile syntax :

* arm-gcc-cortex-m.compiler : GCC compiler for ARM Cortex-M CPUs
* arm-iar.compiler : IAR Systems compiler for ARM CPUs
* arm-keil.compiler : ARM Keil compiler for ARM CPUs
* mingw.compiler : MingGW compiler for Windows

#### Building using the build python script

The build.py script at the root of Nano Build can be use to build multiple project at a time or to build projects without having to navigate in the Makefile directories.


usage: build.py [-h] -t target -p package [package ...] -c command
                [command ...] [-j jobs] [-l] [-v] [-d]

Nano Build tool v1.0

optional arguments:
  -h, --help            show this help message and exit
  -t target             [string] Target name
  -p package [package ...]
                        [string] Packages to build
  -c command [command ...]
                        [string] Build command, ex: all|all+|clean|clean+
  -j jobs               [int] Number of build jobs
  -l                    List all available packages and targets
  -v                    Verbose mode
  -d                    Header dependency check




The following command will list all the available projects and all the available build targets (a target is the association of a compiler and a CPU):

build.py -l

Packages list:
 - apps.demo_blink
 - apps.demo_blink_cpp
 - apps.demo_blink_join
 - apps.demo_blink_rr
 - apps.demo_blink_timer
 - apps.demo_mpu_cortex_m
 - bsps.bsp_emf32zg222
 - bsps.bsp_kl25z128
 - bsps.bsp_lpc1768
 - bsps.bsp_stm32f407
 - bsps.bsp_stm32f767
 - bsps.bsp_windows
 - libs.nano-os
 - libs.nano-os-cortexm-swo-link
 - libs.nano-os-cpp-api
 - libs.nano-os-segger-rtt-link

Target list:
 - arm-gcc-efm32zg222
 - arm-gcc-kl25z128
 - arm-gcc-lpc1768-mpu
 - arm-gcc-lpc1768
 - arm-gcc-stm32f407
 - arm-gcc-stm32f767-mpu
 - arm-gcc-stm32f767
 - arm-iar-kl25z128
 - arm-iar-lpc1768
 - arm-iar-stm32f407
 - arm-keil-kl25z128
 - mingw-windows


The following command will build the demo_blink application for the LPC1768 CPU using the GCC compiler:

build -t arm-gcc-lpc1768 -p apps.demo_blink -c all+

The following command will clean the demo_blink application for the LPC1768 CPU using the GCC compiler:

build -t arm-gcc-lpc1768 -p apps.demo_blink -c clean+

The following command will build all the demo applications for the LPC1768 CPU using the GCC compiler:
 
build -t arm-gcc-lpc1768 -p apps.* -c all+


The build output can be found in the following folder : build/apps/app_name/bin/target_name

#### Building directly from the Makefiles

To build a project directly from a makefile, you have to open a terminal in the Makefile directory and use the following command lines.

The following command will build the demo_blink application for the LPC1768 CPU using the GCC compiler:

make TARGET=arm-gcc-lpc1768 all+

The following command will clean the demo_blink application for the LPC1768 CPU using the GCC compiler:

make TARGET=arm-gcc-lpc1768 clean+

## Demos applications

### Demo environment

The demo applications have been tested on the following boards:

* Freescale Freedom Board for KL25Z128 (Cortex-M0+)
* Gecko ZG EFM32ZG222 Starter Kit (Cortex-M0+)
* Embest NXP LPC1768 Evalboard (Cortex-M3 with MPU)
* Embedded Artists NXP LPC1788 OEM Board (Cortex-M3 with MPU)
* ST STM32F407 Evalboard (Cortex-M4 with FPU and MPU)
* ST NUCLEO STM32F767 (Cortex-M7 with FPU and MPU)

using the following compilers:

* GCC 5.2 for ARM Cortex-M compiler (GNU ARM Embedded Toolchain - https://launchpad.net/gcc-arm-embedded)
* IAR 6.4 for ARM compiler (IAR Systems - https://www.iar.com/iar-embedded-workbench/)
* Keil MDK 5.3 for ARM compiler (ARM Keil - http://www2.keil.com/mdk5/)
* MinGW 7.1 for Windows (MinGW - http://www.mingw.org/)

### Demos

* demo_blink : 2 tasks which makes leds blink using semaphore and task sleep
* demo_blink_join: 1 task which spawns tasks to make the leds blink and which use task join functionality to synchronize
* demo_blink_rr : 4 with the same priority using round robin scheduling to make the leds blink
* demo_blink_timer : 2 tasks using timer and waitable timer to make the led blink
* demo_blink_cpp : same as demo_blink_timer but using the CPP API
* demo_mpu_cortex_m : 2 tasks which makes led blink with MPU enabled
