

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = ./include
ASFLAGS = -m68010 --defsym __mc68010__=1
CFLAGS = -I$(INCLUDE) -m68010 -nostartfiles -nostdlib -g -mpcrel -Os
#CFLAGS += -z max-page-size=0x04	# reduce the elf output size by reducing the alignment

ASLISTING = -alh


MONITOR_OBJS = \
	src/monitor/crt0.o \
	src/monitor/monitor.o \
	src/monitor/vectors.o \
	src/monitor/tty_68681.o \
	libc-68k-none.a

KERNEL_OBJS = \
	include/kernel/syscall.h \
	src/kernel/start.o \
	src/kernel/main.o \
	src/kernel/interrupts.o \
	src/kernel/syscall_entry.o \
	src/kernel/syscall.o \
	src/kernel/process.o \
	src/kernel/driver.o \
	src/kernel/fs/vfs.o \
	src/kernel/fs/vnode.o \
	src/kernel/fs/filedesc.o \
	src/kernel/fs/mallocfs/mallocfs.o \
	src/kernel/drivers/tty_68681.o \
	src/kernel/sh.o \
	libc-68k-none.a \
	libc-68k-os.a		# For unistd syscall functions only

LIBC_BASE_OBJS = $(filter-out src/libc/arch/*, \
	$(patsubst %.c,%.o, $(wildcard src/libc/**/*.c)) \
	$(patsubst %.s,%.o, $(wildcard src/libc/**/*.s)) \
)

ARCH = 68k-os

LIBC_ARCH_OBJS = \
	$(patsubst %.c,%.o, $(wildcard src/libc/arch/$(ARCH)/**/*.c)) \
	$(patsubst %.s,%.o, $(wildcard src/libc/arch/$(ARCH)/**/*.s))


libc-68k-none.a: $(LIBC_BASE_OBJS)
libc-68k-os.a: $(LIBC_BASE_OBJS) $(LIBC_ARCH_OBJS)

#monitor.bin: $(MONITOR_OBJS)
#monitor.elf: $(MONITOR_OBJS)

#kernel.bin: $(KERNEL_OBJS)
#kernel.elf: $(KERNEL_OBJS)



all: monitor.bin kernel.bin kernel.load


welcome:
	$(AS) -m68000 -o welcome.elf welcome.s
	$(OBJCOPY) -O binary welcome.elf welcome.bin

monitor.bin: $(MONITOR_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1200 -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tmonitor.ld -o $@ $^
	hexdump -v -e '/1 "0x%02X, "' $@ > output.txt

kernel.bin: $(KERNEL_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -Wl,--oformat=binary -o $@ $^
	# TODO this works, but I'd like to get rid of all the special sections.  The old linker script worked fine
	#$(OBJCOPY) -O binary  -j .text -j .text.startup -j .rodata -j .rodata.str1.1 -j .data -j .data.rel -j .data.rel.local -j .bss $@.elf $@

%.load: %.bin
	echo "load" > $@
	wc -c $^ | awk '{printf "%04X", $$1}' >> $@
	hexdump -v -e '/1 "%02X"' $^ >> $@

%.send: %.bin
	echo "send $(notdir $(basename $@))" > $@
	wc -c $^ | awk '{printf "%04X", $$1}' >> $@
	hexdump -v -e '/1 "%02X"' $^ >> $@


# TODO this was to test the optimization
#optest: libc68k.a src/monitor/tty_68681.o  src/commands/tty.o src/commands/test.bin
#	cat vectors.data src/commands/test.bin > test.bin
#	hexdump -v -e '/1 "0x%02X, "' test.bin > output.txt


# This is specifically for commands
%.bin: %.o libc-68k-os.a
	$(CC) $(CFLAGS) --entry=main -Ttext=0 -o $@.elf $< libc-68k-os.a
	$(OBJCOPY) -O binary -j .text -j .rodata -j .data $@.elf $@

#%.bin: %.elf
#	#$(OBJCOPY) -O binary -j .text -j .data -j .rodata $^ $@
#	#hexdump -v -e '/1 "0x%02x, "' $@ > output.txt
#	#$(OBJCOPY) -O binary -j .data $^ $@.data
#	#hexdump -v -e '/1 "0x%02x, "' $@.data > output.data.txt
#
#	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -o $@ $^
#	hexdump -v -e '/1 "0x%02x, "' $@ > output.txt
#	hexdump -v -e '/1 "%02x"' $@ > kernel.bin.txt
#
#%.elf: $^
#	# Link into an elf file for inspection
#	# With the new linker script, this output isn't used
#	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1200 -o $@ $^


%.a: $^
	$(AR) rc $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

dump-main:
	$(OBJDUMP) -D main.elf

clean:
	find . \( -name "*.o" -or -name "*.o64" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.load" \) -delete -print


test: test.o64 src/stdio.o64
	gcc -o $@ $^

%.o64: %.c
	gcc -c -o $@ $^
