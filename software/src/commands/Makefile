
ROOT = ../..
BUILD = $(ROOT)/build
$(shell mkdir -p $(BUILD)/bin)

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
STRIP = m68k-linux-gnu-strip
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = $(ROOT)/include
ASFLAGS = -m68010 --defsym __mc68010__=1
CFLAGS = -I$(INCLUDE) -m68010 -nostartfiles -nostdlib -g -mpcrel -Os
LDFLAGS = -Wl,--build-id=none --entry=main -Ttext=0

# reduce the elf output size by reducing the alignment
CFLAGS += -z max-page-size=0x10


COMMANDS = 		\
	sh.send 	\
	cat.send	\
	cp.send		\
	ls.send		\
	ln.send		\
	mv.send		\
	rm.send		\
	mkdir.send	\
	echo.send	\
	chmod.send	\
	ps.send		\
	kill.send	\
	mount.send	\
	umount.send	\
	sync.send	\
	tar.send	\
	hello.send	\
	test.send	\
	daemontest.send

all: $(ROOT)/libc-68k-os.a $(COMMANDS)


sh.bin: CFLAGS += -DONEBINARY -DADDMAIN
#sh.bin: sh.o ps.o


$(ROOT)/libc-68k-os.a:
	make -C $(ROOT)/src/libc clean $@

%.send: %.bin
	#$(ROOT)/tools/make-send-file  $^ $@
	echo "send $*" > $@
	wc -c $^ | awk '{printf "%04X", $$1}' >> $@
	hexdump -v -e '/1 "%02X"' $^ >> $@

%.bin: %.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $*.elf $^ $(ROOT)/libc-68k-os.a
	$(STRIP) $*.elf -o $(BUILD)/bin/$*
	$(OBJCOPY) -O binary -j .text -j .got -j .rodata -j .data $*.elf $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

clean:
	@find . \( -name "*.o" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.send" -or -name "*.exe" -or -name "*.o64" \) -delete


CFLAGS_X86 = -g
sh.exe: CFLAGS_X86 += -DLINUXHOST -DONEBINARY -DADDMAIN

%.exe: %.o64
	gcc $(CFLAGS_X86) -o $*.exe $^

%.o64: %.c
	gcc $(CFLAGS_X86) -c -o $@ $<


