

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = ./include
ASFLAGS = -m68010 --defsym __mc68010__=1
CFLAGS = -I$(INCLUDE) -m68010 -nostartfiles -nostdlib -g -Os
#-fanalyzer
CFLAGS += -z max-page-size=0x10 # reduce the elf output size by reducing the alignment

ASLISTING = -alh


BOOT_OBJS = \
	src/boot/boot.o \
	libc-68k-none.a


MONITOR_OBJS = \
	src/monitor/crt0.o \
	src/monitor/monitor.o \
	src/monitor/vectors.o \
	src/monitor/tty_68681.o \
	libc-68k-none.a


DEVFS_OBJS = \
	src/kernel/fs/devfs/devfs.o

PROCFS_OBJS = \
	src/kernel/fs/procfs/procfs.o

MALLOCFS_OBJS = \
	src/kernel/fs/mallocfs/alloc.o \
	src/kernel/fs/mallocfs/mallocfs.o

MINIXFS_OBJS = \
	src/kernel/fs/minix/bitmaps.o \
	src/kernel/fs/minix/minix.o

FILESYSTEM_OBJS = \
	$(PROCFS_OBJS) \
	$(MINIXFS_OBJS)

DRIVER_OBJS = \
	src/kernel/drivers/tty_68681.o \
	src/kernel/drivers/mem.o \
	src/kernel/drivers/ata.o

KERNEL_OBJS = \
	include/kernel/syscall.h \
	src/kernel/start.o \
	src/kernel/main.o \
	src/kernel/syscall_entry.o \
	src/kernel/syscall.o \
	src/kernel/interrupts.o \
	src/kernel/filedesc.o \
	src/kernel/time.o \
	src/kernel/driver.o \
	src/kernel/proc/scheduler.o \
	src/kernel/proc/process.o \
	src/kernel/proc/tasks.o \
	src/kernel/proc/memory.o \
	src/kernel/proc/signal.o \
	src/kernel/proc/binaries.o \
	src/kernel/mm/kmalloc.o \
	src/kernel/fs/vfs.o \
	src/kernel/fs/nop.o \
	src/kernel/fs/pipe.o \
	src/kernel/fs/device.o \
	src/kernel/fs/access.o \
	src/kernel/fs/fileptr.o \
	src/kernel/fs/bufcache.o \
	$(FILESYSTEM_OBJS) \
	$(DRIVER_OBJS) \
	src/commands/init.o \
	src/commands/sh.o \
	libc-68k-os.a
#	src/tests/kernel/tests.o \


all: monitor.bin monitor.load boot.bin boot.load kernel.bin kernel.load

commands:
	make -C src/commands clean all

libc-68k-none.a libc-68k-os.a:
	make -C src/libc ../../$@


welcome monitor.bin boot.bin: CFLAGS += -mpcrel

welcome:
	$(AS) -m68000 -o welcome.elf welcome.s
	$(OBJCOPY) -O binary welcome.elf welcome.bin


monitor.bin: $(MONITOR_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1000 -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tmonitor.ld -o $@ $^
	hexdump -v -e '/1 "0x%02X, "' $@ > output.txt


boot.load: boot.bin
	tools/make-load-file $^ $@ 180000

boot.bin: $(BOOT_OBJS)
	$(CC) $(CFLAGS) --entry=main -Ttext=0x0000 -o $@.elf $^
	$(CC) $(CFLAGS) --entry=main -Ttext=0x0000 -Wl,--oformat=binary -o $@ $^


kernel.bin: CFLAGS += -DONEBINARY
kernel.bin: $(KERNEL_OBJS)
	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -o $@.elf $^
	$(CC) $(CFLAGS) --entry=_start -Tkernel.ld -Wl,--oformat=binary -o $@ $^


%.load: %.bin
	tools/make-load-file $^ $@

%.send: %.bin
	tools/make-send-file $^ $@

%.a: $^
	$(AR) rc $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

clean:
	find . \( -name "*.o" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.load" \) -delete -print


BLOCKS = 128
IMAGE = minix-test.img
LOOPBACK = /dev/loop8
MOUNTPOINT = build

create-image:
	dd if=/dev/zero of=$(IMAGE) bs=1K count=$(BLOCKS)
	sudo losetup $(LOOPBACK) $(IMAGE)
	sudo mkfs.minix -1 -n 14 $(LOOPBACK) $(BLOCKS)
	sudo losetup -d $(LOOPBACK)

mount-image:
	sudo losetup $(LOOPBACK) $(IMAGE)
	sudo mount -t minix $(LOOPBACK) $(MOUNTPOINT)

umount-image:
	sudo umount $(LOOPBACK)
	sudo losetup -d $(LOOPBACK)

