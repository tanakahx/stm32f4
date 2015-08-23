CFLAGS += -march=armv7-m -mcpu=$(CPU) -mthumb -Icpu/$(CPU) -D STM32F407xx
LDFLAGS += -T cpu/$(CPU)/arm.ld
OBJS += cpu/$(CPU)/vector.o cpu/$(CPU)/system_stm32f4xx.o
