
DSTPATH=out

all: $(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a
	make -C test

clean:
	rm -rf $(DSTPATH)
	make -C test clean
	make -C xcb clean

distclean: clean
	rm -rf *~
	make -C xcb distclean
	make -C test distclean

.PHONY: all clean distclean

$(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a:
	make -C xcb xproto.h # must be called first to generate xproto.???.c files
	make -C xcb ../$@

