
GLOWORM = ../gloworm

include $(GLOWORM)/config.mk

ASLISTING = -alh


MONITOR_OBJS = \
	crt0.o \
	monitor.o \
	vectors.o \
	tty_68681.o \
	$(GLOWORM)/libc-68k-none.a

all: monitor.bin monitor.load

term:
	pyserial-miniterm /dev/ttyUSB0 38400

$(GLOWORM)/libc-68k-none.a $(GLOWORM)/libc-68k-os.a:
	make -C $(GLOWORM)/src/libc ../../$@


monitor.bin: CFLAGS += -mpcrel


monitor.bin: $(MONITOR_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tmonitor.ld -o $@.elf $^
	$(CC) $(CFLAGS) $(LDFLAGS) --entry=_start -Tmonitor.ld -Wl,--oformat=binary -o $@ $^
	hexdump -v -e '/1 "0x%02X, "' $@ > output.txt

%.load: %.bin
	tools/make-load-file $^ $@ $(LOAD_ARGS)

%.send: %.bin
	tools/make-send-file $^ $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

clean:
	make -C $(GLOWORM) clean
	find . \( -name "*.o" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.load" \) -delete -print

