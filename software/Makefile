
ROOT = .

include $(ROOT)/config.mk

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
	src/kernel/fs/procfs/data.o \
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
	src/kernel/drivers/tty.o \
	src/kernel/drivers/partition.o \
	src/kernel/drivers/mem.o \
	src/kernel/drivers/ata.o

KERNEL_OBJS = \
	include/kernel/syscall.h \
	src/kernel/start.o \
	src/kernel/main.o \
	src/kernel/syscall_entry.o \
	src/kernel/syscall.o \
	src/kernel/interrupts.o \
	src/kernel/bh.o \
	src/kernel/time.o \
	src/kernel/driver.o \
	src/kernel/proc/scheduler.o \
	src/kernel/proc/process.o \
	src/kernel/proc/timer.o \
	src/kernel/proc/tasks.o \
	src/kernel/proc/memory.o \
	src/kernel/proc/signal.o \
	src/kernel/proc/filedesc.o \
	src/kernel/proc/binaries.o \
	src/kernel/proc/select.o \
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
	src/kernel/net/packet.o \
	src/kernel/net/socket.o \
	src/kernel/net/protocol.o \
	src/kernel/net/protocol/inet_af.o \
	src/kernel/net/protocol/ipv4.o \
	src/kernel/net/protocol/udp.o \
	src/kernel/net/protocol/icmp.o \
	src/kernel/net/protocol/tcp.o \
	src/kernel/net/if.o \
	src/kernel/net/if/slip.o \
	src/commands/init.o \
	src/commands/sh.o \
	libc-68k-os.a
#	src/tests/kernel/tests.o \


all: monitor.bin monitor.load boot.bin boot.load kernel.bin kernel.load
build-system: kernel.bin commands devicefiles otherfiles

commands:
	make -C src/commands clean all

devicefiles:
	sudo mkdir -p $(BUILD)/dev
	sudo mknod $(BUILD)/dev/tty0 c 2 0
	sudo mknod $(BUILD)/dev/tty1 c 2 1
	sudo mknod $(BUILD)/dev/mem0 c 3 0
	sudo mknod $(BUILD)/dev/ata0 c 4 0

otherfiles:
	sudo mkdir -p $(BUILD)/etc
	sudo mkdir -p $(BUILD)/proc
	sudo mkdir -p $(BUILD)/home
	sudo mkdir -p $(BUILD)/media
	sudo cp -r etc/* $(BUILD)/etc


libc-68k-none.a libc-68k-os.a:
	make -C src/libc ../../$@


welcome monitor.bin boot.bin: CFLAGS += -mpcrel

welcome:
	$(AS) -m68000 -o welcome.elf src/welcome.s
	$(OBJCOPY) -O binary welcome.elf welcome.bin


monitor.bin: $(MONITOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tmonitor.ld -o $@.elf $^
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tmonitor.ld -Wl,--oformat=binary -o $@ $^
	hexdump -v -e '/1 "0x%02X, "' $@ > output.txt


boot.bin: $(BOOT_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=main -Ttext=0x0000 -o $@.elf $^
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=main -Ttext=0x0000 -Wl,--oformat=binary -o $@ $^


kernel.bin: CFLAGS += -DONEBINARY
kernel.bin: $(KERNEL_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tkernel.ld -o $@.elf $^
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tkernel.ld -Wl,--oformat=binary -o $@ $^
	mkdir -p $(BUILD)
	cp $@ $(BUILD)


boot.load: LOAD_ARGS = 180000
#kernel.load: LOAD_ARGS = 100000 ata0


%.load: %.bin
	tools/make-load-file $^ $@ $(LOAD_ARGS)

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


# TODO 128 is just barely enough for 20 commands, kernel, devfiles
#BLOCKS = 128
BLOCKS = 20480
IMAGE = minix-build.img
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

