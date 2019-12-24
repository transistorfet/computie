

AS = m68k-linux-gnu-as
CC = m68k-linux-gnu-gcc
OBJDUMP = m68k-linux-gnu-objdump
OBJCOPY = m68k-linux-gnu-objcopy
INCLUDE = ./include
CFLAGS = -I$(INCLUDE) -m68000 -nostartfiles -nostdlib -g

ASLISTING = -alh


all: main.bin

main.bin: main.elf
main.elf: src/crt0.o src/main.o src/string.o src/stdio.o src/dev/tty_arduino.o

welcome:
	m68k-linux-gnu-as -m68000 -o welcome.elf welcome.s
	m68k-linux-gnu-objcopy -O binary welcome.elf welcome.bin


%.bin: $^
	#hexdump -v -e '/1 "0x%02x, "' $<; echo ""
	$(OBJCOPY) -O binary -j .text -j .data -j .rodata $^ $@
	hexdump -v -e '/1 "0x%02x, "' $@ > output.txt

%.elf: $^
	$(CC) $(CFLAGS) --entry=_start -Ttext=0x0000 -Tdata=0x1000 -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) -c -o $@ $<

dump-main:
	$(OBJDUMP) -D main.elf

clean:
	find . \( -name "*.o" -or -name "*.bin" -or -name "*.elf" \) -delete -print


test: test.o64 src/stdio.o64
	gcc -o $@ $^

%.o64: %.c
	gcc -c -o $@ $^
