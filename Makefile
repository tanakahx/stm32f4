# CPU := arm926ej-s
CPU := cortex-m3
# CPU := cortex-a9

ifeq ($(CPU), cortex-m3)
MACHINE := lm3s6965evb
VMA := 0x00000000
endif

CC := arm-linux-gnueabi-gcc #arm-none-eabi-gcc
CFLAGS = -Wall -fno-builtin -Isrc

LD := ld-arm
LDFLAGS =

OBJCOPY := arm-linux-gnueabi-objcopy #objcopy-arm

OBJS := src/main.o src/lib.o

include cpu/$(CPU)/Makefile

TARGET := image

$(TARGET): $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.S
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: dis run clean

dis:
	objdump-arm -D -b binary -m arm --adjust-vma=$(VMA) $(TARGET)

run:
	qemu-system-arm -M $(MACHINE) -nographic -kernel $(TARGET)

flash:
	st-flash --reset write $(TARGET) 0x8000000

clean:
	-@$(foreach obj, */*/*.o, rm $(obj);)
	-rm $(TARGET) $(TARGET).elf
