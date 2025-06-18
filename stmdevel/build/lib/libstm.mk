ROOTDIR = ..
include $(ROOTDIR)/Makefile.inc

# Default Architecture 
# (normally overridden by make command line: "make ARCH=STM32F407xx -f thisfile")
ARCH = STM32L476xx

TARGET=lib$(ARCH).a

DRIVEROBJS= 

VPATH = ${STCUBEBASE}/Drivers/STM32L4xx_HAL_Driver/Src/ \
        $(STCUBEBASE)/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/gcc/ \
        $(STCUBEBASE)/Drivers/CMSIS/Device/ST/STM32L4xx/Source/Templates/ \
        $(ROOTDIR)/src/STM32L476xx_nucleo

ifeq ($(ARCH),STM32L476xx)
  BSPOBJS = startup_stm32l476xx.o \
            system_stm32l4xx.o
endif

OBJS = $(addsuffix .$(ARCH), $(DRIVEROBJS)) $(addsuffix .$(ARCH), $(BSPOBJS))

CFLAGS = $(PRJCFLAGS) -D$(ARCH) -I. -I$(INCLUDEDIR)

.PHONY : clean all depend install uninstall

all: $(TARGET)

%.o.$(ARCH) : %.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

%.o.$(ARCH): %.s
	$(COMPILE.s) -o $@ $<

pdm_filter.o.$(ARCH): $(ROOTDIR)/src/stm_bsp_modified/libPDMFilter_CM4F_GCC.a
	$(AR) -xv $< pdm_filter.o
	mv pdm_filter.o pdm_filter.o.$(ARCH)

${TARGET}: ${OBJS}
	${AR} rc ${TARGET} ${OBJS}
	${RANLIB} ${TARGET}

install: $(TARGET)
	mkdir -p $(INSTALLLIB)
	cp $(TARGET) $(INSTALLLIB)

uninstall:
	rm $(INSTALLLIB)/$(TARGET)

clean:
	rm -f *.o ${TARGET}
