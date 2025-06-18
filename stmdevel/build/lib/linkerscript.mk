ROOTDIR = ..
include $(ROOTDIR)/Makefile.inc

.PHONY : clean all install uninstall

all: STM32L476xx_FLASH.ld

STM32L476xx_FLASH.ld: $(ROOTDIR)/src/STM32L476xx_nucleo/STM32L476RGTx_FLASH.ld
	cp $(ROOTDIR)/src/STM32L476xx_nucleo/STM32L476RGTx_FLASH.ld STM32L476xx_FLASH.ld

install: all
	mkdir -p $(INSTALLLIB)
	cp STM32L476xx_FLASH.ld $(INSTALLLIB)

uninstall:
	rm $(INSTALLLIB)/STM32L476xx_FLASH.ld

clean:
	rm -f *.ld
