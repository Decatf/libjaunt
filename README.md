# libjaunt
A library to catch illegal opcode system calls for an ARM Cortex-A9 CPU. The CPU registers are passed on to another library (such as tcg-arm) which can emulate the opcode.

## How to use this
Inject this library by using `LD_PRELOAD` or load it dynamically using `dlopen`.
