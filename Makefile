
DSTPATH=out
# only needed to install the library for Watcom
WATCOM=
# only needed to install the library for IA-16 GCC
IA16GCC=

all: $(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a
	make -C test

watcom: $(DSTPATH)/watcom/xcblib.lib
	make -C test watcom

gcc: $(DSTPATH)/gcc/libxcb.a
	make -C test gcc

native:
	make -C test native

clean:
	rm -rf $(DSTPATH)
	make -C test clean
	make -C xcb clean

distclean: clean
	rm -rf *~
	make -C xcb distclean
	make -C test distclean

install: install_gcc install_watcom

install_gcc:
	cp $(DSTPATH)/gcc/libxcb.a $(IA16GCC)/lib/
	mkdir -p $(IA16GCC)/include/xcb/
	cp xcb/xcb.h xcb/xcb_dos.h xcb/xproto.h $(IA16GCC)/include/xcb/

install_watcom:
	cp $(DSTPATH)/watcom/xcblib.lib $(WATCOM)/lib286/
	mkdir -p $(WATCOM)/h/xcb/
	cp xcb/xcb.h xcb/xcb_dos.h xcb/xproto.h $(WATCOM)/h/xcb/

.PHONY: all clean distclean watcom gcc native install install_gcc install_watcom

$(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a: xcb/xcb_conn.c xcb/xcb_dos.c xcb/xcb_ext.c xcb/xcb_in.c xcb/xcb_list.c xcb/xcb_util.c xcb/xcb_xid.c xcb/xcb_dos.h
	make -C xcb xproto.h # must be called first to generate xproto.???.c files
	make -C xcb ../$@

