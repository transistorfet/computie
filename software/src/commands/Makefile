
ROOT = ../..

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = $(ROOT)/include
ASFLAGS = -m68010 --defsym __mc68010__=1
CFLAGS = -I$(INCLUDE) -m68010 -nostartfiles -nostdlib -g -fPIE -Os
#-fanalyzer
#CFLAGS += -z max-page-size=0x04	# reduce the elf output size by reducing the alignment


COMMANDS = \
	hello.send	\
	sh.send \
	daemontest.send \
	test.send

ARCH = 68k-os


$(ROOT)/libc-68k-os.a:
	make -C $(ROOT)/src/libc libc-68k-os.a


commands: $(COMMANDS)


%.send: %.bin
	echo "send $(notdir $(basename $@))" > $@
	wc -c $^ | awk '{printf "%04X", $$1}' >> $@
	hexdump -v -e '/1 "%02X"' $^ >> $@


%.bin: %.o $(ROOT)/libc-68k-os.a
	$(CC) $(CFLAGS) --entry=main -Ttext=0 -o $@.elf $< $(ROOT)/libc-68k-os.a
	$(OBJCOPY) -O binary -j .text -j .got -j .rodata -j .data $@.elf $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -c -o $@ $<

clean:
	find . \( -name "*.o" -or -name "*.o64" -or -name "*.a" -or -name "*.bin" -or -name "*.elf" -or -name "*.load" \) -delete -print

