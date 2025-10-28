
DSTPATH=out

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

.PHONY: all clean distclean watcom gcc native

$(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a: xcb/xcb_conn.c xcb/xcb_dos.c xcb/xcb_ext.c xcb/xcb_in.c xcb/xcb_list.c xcb/xcb_util.c xcb/xcb_xid.c xcb/xcb_dos.h
	make -C xcb xproto.h # must be called first to generate xproto.???.c files
	make -C xcb ../$@

