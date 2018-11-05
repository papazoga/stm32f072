CFLAGS=-mcpu=cortex-m0plus -mthumb -g
ASFLAGS=-mcpu=cortex-m0plus -mthumb
OBJCOPY=arm-none-eabi-objcopy
OBJS=start.o regs.o interrupt.o main.o
LDFLAGS=-Tstm32f07x.ld
CC=arm-none-eabi-gcc
AS=arm-none-eabi-as
LD=arm-none-eabi-ld
OPENOCD=openocd
OPENOCDARGS=-f interface/ftdi/dp_busblaster_kt-link.cfg \
	    -c "adapter_khz 6000; transport select swd" \
	    -f target/stm32f0x.cfg

hexfile: firmware.hex
binfile: firmware.bin

firmware.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

%.hex: %.elf
	$(OBJCOPY) -I elf32-littlearm -O ihex $< $@

%.bin: %.elf
	$(OBJCOPY) -I elf32-littlearm -O binary $< $@

regs.s: regs.txt
	sed $< -e 's/\([^ ]*\) \(.*\)$$/\1 = \2\n.global \1\n/' >$@

regs.o: regs.s
	$(AS) $(ASFLAGS) -mno-warn-syms $< -o $@

clean:
	rm -f $(OBJS) *.hex *.elf *.bin

flash: firmware.hex
	$(OPENOCD) $(OPENOCDARGS) -c "init" \
				  -c "halt" \
				  -c "flash write_image erase firmware.hex" \
				  -c "reset run" \
				  -c "shutdown"
