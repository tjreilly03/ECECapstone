
ROOTDIR = .

include $(ROOTDIR)/Makefile.inc

DIRS = $(INCLUDEDIR) \
       $(LIBDIR)

.Phony: all install uninstall clean

all clean install uninstall:
	$(foreach subdir, $(DIRS), $(MAKE) -C $(subdir) $@;)


