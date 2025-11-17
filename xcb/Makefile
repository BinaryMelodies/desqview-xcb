
XPROTO=$(wildcard xproto.*.c)
GENOBJS=bigreq.o xc_misc.o $(patsubst %.c,%.o,$(XPROTO))
GENHDRS=bigreq.h xc_misc.h xproto.h
LIBOBJS=xcb_util.o xcb_dos.o xcb_conn.o xcb_out.o xcb_in.o xcb_ext.o xcb_xid.o xcb_list.o
OBJECTS=$(GENOBJS) $(LIBOBJS)
DSTPATH=../out

all: $(DSTPATH)/watcom/xcblib.lib $(DSTPATH)/gcc/libxcb.a

clean:
	rm -rf $(patsubst %.o,%.c,$(GENOBJS)) $(GENHDRS) xproto.*.c *.err

distclean: clean
	rm -rf *~

.PHONY: all clean distclean

$(DSTPATH)/watcom/%.o: %.c $(GENHDRS)
	mkdir -p `dirname $@`
	wcl -q -bcl=dos -c $< -fo=$@ -dHAVE_CONFIG_H=1

$(DSTPATH)/watcom/l/%.o: %.c $(GENHDRS)
	mkdir -p `dirname $@`
	wcl -q -bcl=dos -c $< -fo=$@ -dHAVE_CONFIG_H=1 -ml

$(DSTPATH)/gcc/%.o: %.c $(GENHDRS)
	mkdir -p `dirname $@`
	ia16-elf-gcc -c $< -o $@ -DHAVE_CONFIG_H=1 -Os

$(DSTPATH)/watcom/xcblib.lib: $(patsubst %,$(DSTPATH)/watcom/%,$(OBJECTS))
	rm -f $@
	wlib -q $@ $(patsubst %,+%,$^)

$(DSTPATH)/watcom/xcblibl.lib: $(patsubst %,$(DSTPATH)/watcom/l/%,$(OBJECTS))
	rm -f $@
	wlib -q $@ $(patsubst %,+%,$^)

$(DSTPATH)/gcc/libxcb.a: $(patsubst %,$(DSTPATH)/gcc/%,$(OBJECTS))
	ar rcs $@ $^

%.c %.h: xcb-proto/%.xml
	python3 ./c_client.py -c "libxcb 1.17.0" -l "X Version 11" -s 3 $<

