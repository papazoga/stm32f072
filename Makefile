CFLAGS=-mcpu=cortex-m0plus -mthumb
ASFLAGS=-mcpu=cortex-m0plus -mthumb
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
LD=arm-none-eabi-ld
OBJS=start.o interrupt.o
LDFLAGS=-Tstm32f07x.ld

all: firmware.hex

firmware.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o firmware.elf

firmware.hex: firmware.elf
	$(OBJCOPY) -I elf32-littlearm -O ihex firmware.elf firmware.hex

clean:
	rm -f $(OBJS) firmware.elf

