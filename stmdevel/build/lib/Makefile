ROOTDIR = ..
include $(ROOTDIR)/Makefile.inc

# Removed libstm.mk \ libcmsis_dsp.mk \ 
ARCHMKFILES = libece486.mk \
              libstm.mk \
              libcmsis_dsp.mk \
              linkerscript.mk

.PHONY : clean all install uninstall

all clean install uninstall:
	$(foreach mkfile,$(ARCHMKFILES), $(MAKE) ARCH=STM32L476xx -f $(mkfile) $@;)
