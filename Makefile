TARGET=ECECapstone_distortion

OBJS  = ECECapstone_distortion.o soft_clip.o hard_clip.o get_values.o \
        stm32l4xx_hal.o stm32l4xx_hal_adc.o stm32l4xx_hal_adc_ex.o \
        stm32l4xx_hal_rcc.o stm32l4xx_hal_gpio.o stm32l4xx_hal_cortex.o \
        stm32l4xx_hal_pwr.o stm32l4xx_hal_flash.o stm32l4xx_hal_flash_ex.o


INSTALLDIR = /usr/local/stmdev/
#INSTALLDIR = ../../build


ARCH = STM32L476xx

CC=arm-none-eabi-gcc
LD=arm-none-eabi-ld
AS=arm-none-eabi-as
OBJCOPY=arm-none-eabi-objcopy

INCDIRS = \
  -I. \
  -I./Drivers/STM32L4xx_HAL_Driver/Inc \
  -I./Drivers/CMSIS/Device/ST/STM32L4xx/Include \
  -I./Drivers/CMSIS/Include \
  -I$(INSTALLDIR)/include


LIBDIRS = -L$(INSTALLDIR)/lib

LIBS=  -lece486_$(ARCH) -l$(ARCH) -lm -lcmsis_dsp_$(ARCH) 

LINKSCRIPT = $(INSTALLDIR)/lib/$(ARCH)_FLASH.ld

CFLAGS = -mcpu=cortex-m4 -mthumb -O3 -Wall  \
         -fomit-frame-pointer -fno-strict-aliasing -fdata-sections \
         -DARM_MATH_CM4 -D$(ARCH) \
         -mfpu=fpv4-sp-d16 -mfloat-abi=hard $(INCDIRS) \
         -fsingle-precision-constant -ffunction-sections \
		 -I./Drivers/CMSIS/Include \
		-I./Drivers/CMSIS/Device/ST/STM32L4xx/Include \
		-I./Drivers/STM32L4xx_HAL_Driver/Inc

%.o: Drivers/STM32L4xx_HAL_Driver/Src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

LDFLAGS = -u _printf_float \
	  -Wl,-T$(LINKSCRIPT) \
	  -static \
          -Wl,--gc-sections $(LIBDIRS)
               
.PHONY : all flash clean debug

all: $(TARGET) $(TARGET).bin

debug : CFLAGS += -DDEBUG -g -Og
debug : LDFLAGS += -Wl,-Map,$(TARGET).map

debug : all

$(TARGET): $(OBJS)
	$(CC)  -o $(TARGET) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS)

$(TARGET).bin: $(TARGET)
	$(OBJCOPY) -Obinary $(TARGET) $(TARGET).bin

flash: $(TARGET).bin
	st-flash write $(TARGET).bin 0x08000000

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).bin $(TARGET).map
