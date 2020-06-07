

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = ./include
CFLAGS = -I$(INCLUDE) -m68000 -nostartfiles -nostdlib -g

ASLISTING = -alh


MONITOR_OBJS = \
	src/monitor/crt0.o \
	src/monitor/monitor.o \
	src/monitor/vectors.o \
	src/monitor/tty_68681.o \
	libc68k.a

KERNEL_OBJS = \
	src/kernel/crt0.o \
	src/kernel/main.o \
	src/kernel/interrupts.o \
	src/kernel/driver.o \
	src/kernel/inode.o \
	src/kernel/filedesc.o \
	src/kernel/drivers/tty_68681.o \
	libc68k.a

libc68k.a: \
	$(patsubst %.c,%.o, $(wildcard src/libc/**/*.c)) \
	$(patsubst %.s,%.o, $(wildcard src/libc/**/*.s))

#monitor.bin: $(MONITOR_OBJS)
#monitor.elf: $(MONITOR_OBJS)

#kernel.bin: $(KERNEL_OBJS)
#kernel.elf: $(KERNEL_OBJS)


all: monitor.bin kernel.bin


welcome:
	m68k-linux-gnu-as -m68000 -o welcome.elf welcome.s
	m68k-linux-gnu-objcopy -O binary welcome.elf welcome.bin

monitor.bin: $(MONITOR_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1200 -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tmonitor.ld -o $@ $^
	hexdump -v -e '/1 "0x%02x, "' $@ > output.txt

kernel.bin: $(KERNEL_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x100000 -Tdata=0x101400 -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -o $@ $^

	# Create load script
	echo "load" > kernel.load
	wc -c kernel.bin | awk '{printf "%04x", $$1}' >> kernel.load
	hexdump -v -e '/1 "%02X"' $@ >> kernel.load

%.bin: $^
	#$(OBJCOPY) -O binary -j .text -j .data -j .rodata $^ $@
	#hexdump -v -e '/1 "0x%02x, "' $@ > output.txt
	#$(OBJCOPY) -O binary -j .data $^ $@.data
	#hexdump -v -e '/1 "0x%02x, "' $@.data > output.data.txt

	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -o $@ $^
	hexdump -v -e '/1 "0x%02x, "' $@ > output.txt
	hexdump -v -e '/1 "%02x"' $@ > kernel.bin.txt

%.elf: $^
	# Link into an elf file for inspection
	# With the new linker script, this output isn't used
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1200 -o $@ $^

%.a: $^
	$(AR) rc $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) -c -o $@ $<

dump-main:
	$(OBJDUMP) -D main.elf

clean:
	find . \( -name "*.o" -or -name "*.o64" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" \) -delete -print


test: test.o64 src/stdio.o64
	gcc -o $@ $^

%.o64: %.c
	gcc -c -o $@ $^
