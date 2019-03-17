# Bare-metal STM32F07x

This project started because I don't like using vendor-provided libraries and IDEs.
The main parts are:

* A linker script (`stm32f07x.ld`)
* Unoptimized startup code (`start.S`)
* Interrupt handler boilerplate (`interrupt.c`)
* SFR declarations

Here's some brief descriptions of these:

## The linker script

This is pretty straightforward. It tells the linker where the various things are located in memory.
In order to support C++, some things need to be added. I don't tend to use C++, so I haven't added
them.

## Startup code

On these processors, this means the interrupt vector table and the reset handler. The reset handler
zeros the BSS and copies the data. It doesn't do much else, because the Cortex-M0 is a simple beast.
It then calls main()

## Interupt handlers

Everything has been set up so that to implement an interrupt handler you just declare it with
the following signature:

```
void __attribute__((interrupt("IRQ"))) _FOO_handler()
```

You can find the appropriate name of the handler in `interrupt.c`.

## SFR declarations

There's a bit of novelty here, compared to CMSIS/STM32Cube. The Makefile uses sed to put the SFRs
where they should go by generating an assembler file (`regs.s`) from a text file (`regs.txt`).
This way, you can declare some structs, add the SFR to the list, and be done. Also, with this scheme,
the SFRs show up as symbols with actual addresses. Thus, unlike STM32Cube which relies on `#define`s,
it's possible to read the registers by name from gdb (if you use gdb), and they are not pointers, but
actual structs. For example, the mode register of GPIO A can be accessed like so:

```
        GPIOA.MODER = 0x00;
```

