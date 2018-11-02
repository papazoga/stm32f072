CFLAGS=-mcpu=cortex-m0plus -mthumb -g
ASFLAGS=-mcpu=cortex-m0plus -mthumb
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
OBJS=start.o regs.o interrupt.o main.o
LDFLAGS=-Tstm32f07x.ld

all: firmware.hex

firmware.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o firmware.elf

firmware.hex: firmware.elf
	$(OBJCOPY) -I elf32-littlearm -O ihex firmware.elf firmware.hex

firmware.bin: firmware.elf
	$(OBJCOPY) -I elf32-littlearm -O binary firmware.elf firmware.bin

clean:
	rm -f $(OBJS) *.hex *.elf *.bin

