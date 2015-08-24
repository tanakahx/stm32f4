ARCH := armv7-m
BOARD := stm32f4

EX := timer_ex

CC := arm-linux-gnueabi-gcc
LD := ld-arm
OBJCOPY := arm-linux-gnueabi-objcopy

CFLAGS = -Wall -fno-builtin -Ilib -I$(EX)
LDFLAGS =
OBJS := lib/lib.o $(EX)/main.o

TARGET := image

all:
	$(MAKE) $(TARGET)

include arch/$(ARCH)/Makefile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJS)
	$(LD) -o $@ $(LDFLAGS) $^

$(TARGET): $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

.PHONY: clean

serial:
	cu -s 115200 -l /dev/ttyUSB0

clean:
	-@$(foreach obj, */*/*/*.o */*.o, rm $(obj);)
	-rm $(TARGET) $(TARGET).elf
