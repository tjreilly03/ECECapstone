
ROOTDIR = ..
include $(ROOTDIR)/Makefile.inc

INCDIRS = ${STCUBEBASE}/Drivers/CMSIS/Device/ST/STM32L4xx/Include \
          ${STCUBEBASE}/Drivers/CMSIS/Include \
          $(ROOTDIR)/src/libece486/

.PHONY : clean all install uninstall

all:
	$(foreach incdir,$(INCDIRS), cp $(incdir)/*.h .;)
	cp $(CMSISBASE)/DSP/Include/arm_math.h .
	cp $(CMSISBASE)/DSP/Include/arm_const_structs.h .
	cp $(CMSISBASE)/DSP/Include/arm_common_tables.h .


install: all
	mkdir -p $(INSTALLINC)
	cp *.h $(INSTALLINC)
	cp -r Legacy $(INSTALLINC)

uninstall:
	rm $(INSTALLINC)/*.h
	rm -rf $(INSTALLINC)/Legacy

clean:
	rm -f *.h
	rm -rf Legacy
