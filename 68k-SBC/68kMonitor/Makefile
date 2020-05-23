

AS = m68k-linux-gnu-as
AR = m68k-linux-gnu-ar
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
RANLIB = m68k-linux-gnu-ranlib
INCLUDE = ./include
CFLAGS = -I$(INCLUDE) -m68000 -nostartfiles -nostdlib -g

ASLISTING = -alh


all: main.elf main.bin

OBJS = src/crt0.o src/main.o src/dev/tty_68681.o libc68k.a
main.bin: $(OBJS)
main.elf: $(OBJS)


libc68k.a: $(patsubst %.c,%.o, $(wildcard src/libc/**/*.c))


welcome:
	m68k-linux-gnu-as -m68000 -o welcome.elf welcome.s
	m68k-linux-gnu-objcopy -O binary welcome.elf welcome.bin


%.bin: $^
	#$(OBJCOPY) -O binary -j .text -j .data -j .rodata $^ $@
	#hexdump -v -e '/1 "0x%02x, "' $@ > output.txt
	#$(OBJCOPY) -O binary -j .data $^ $@.data
	#hexdump -v -e '/1 "0x%02x, "' $@.data > output.data.txt

	$(CC) $(CFLAGS) --entry=_start -Tlinker.ld -o $@ $^
	hexdump -v -e '/1 "0x%02x, "' $@ > output.txt

%.elf: $^
	# Link into an elf file for inspection
	# With the new linker script, this output isn't used
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0020 -Tdata=0x1000 -o $@ $^

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
	find . \( -name "*.o" -or -name "*.o64" -or -name "*.bin" -or -name "*.elf" \) -delete -print


test: test.o64 src/stdio.o64
	gcc -o $@ $^

%.o64: %.c
	gcc -c -o $@ $^
