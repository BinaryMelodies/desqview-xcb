
DSTPATH=../out
TESTS=cursor.c events.c gfx.c text.c wm.c

all: $(patsubst %.c,$(DSTPATH)/watcom/%.exe,$(TESTS)) $(patsubst %.c,$(DSTPATH)/gcc/%.exe,$(TESTS)) $(patsubst %.c,$(DSTPATH)/native/%,$(TESTS))

clean:

distclean: clean
	rm -rf *~

.PHONY: all clean distclean

$(DSTPATH)/watcom/%.o: %.c
	mkdir -p `dirname $@`
	wcl -q -bcl=dos -c $< -fo=$@ -dHAVE_CONFIG_H=1 -i=..

$(DSTPATH)/watcom/%.exe: $(DSTPATH)/watcom/%.o $(DSTPATH)/watcom/xcblib.lib
	wcl -q -bcl=dos $^ -fe=$@

$(DSTPATH)/gcc/%.o: %.c
	mkdir -p `dirname $@`
	ia16-elf-gcc -c $< -o $@ -DHAVE_CONFIG_H=1 -Os -I..

$(DSTPATH)/gcc/%.exe: $(DSTPATH)/gcc/%.o $(DSTPATH)/gcc/libxcb.a
	ia16-elf-gcc $< -o $@ -mcmodel=small -L$(DSTPATH)/gcc -lxcb -li86 -Os

$(DSTPATH)/native/%: %.c
	mkdir -p `dirname $@`
	gcc -o $@ $< -lxcb

