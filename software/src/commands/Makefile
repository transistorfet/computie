
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

# reduce the elf output size by reducing the alignment, since we don't need page alignment yet
# alignment can be turned off with -Wl,-n
LDFLAGS += -z common-page-size=0x10 -z max-page-size=0x10
#LDFLAGS += -Wl,-z,norelro


COMMANDS = 		\
	cat.send	\
	chmod.send	\
	cp.send		\
	date.send	\
	echo.send	\
	hello.send	\
	init.send	\
	kill.send	\
	ln.send		\
	ls.send		\
	mkdir.send	\
	mount.send	\
	mv.send		\
	ns.send		\
	ps.send		\
	rm.send		\
	sh.send		\
	sync.send	\
	tar.send	\
	umount.send	\
	test.send	\
	nettest.send	\
	daemontest.send

all: $(ROOT)/libc-68k-os.a $(COMMANDS)


sh.bin: CFLAGS += -DONEBINARY -DADDMAIN
#sh.bin: sh.o ps.o


$(ROOT)/libc-68k-os.a:
	make -C $(ROOT)/src/libc clean $@

%.send: %.bin
	$(ROOT)/tools/make-send-file $(BUILD)/bin/$* $@ $*

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


