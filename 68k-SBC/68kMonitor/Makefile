

all: welcome.bin

welcome.bin: welcome.o
#welcome.elf: welcome.o
main.bin: main.elf
main.elf: crt0.o main.o

%.bin: $^
	#hexdump -v -e '/1 "0x%02x, "' $<; echo ""
	m68k-linux-gnu-objcopy -O binary -j .text -j .data -j .rodata $^ $@
	hexdump -v -e '/1 "0x%02x, "' $@; echo ""

%.elf: $^
	m68k-linux-gnu-gcc -m68000 -nostdlib --entry=_start -Ttext=0x0000 -Tdata=0x1000 -g -o $@ $^

%.o: %.c
	m68k-linux-gnu-gcc -m68000 -nostdlib -c -g -o $@ $<

%.o: %.s
	m68k-linux-gnu-as -m68000 -alh -c -o $@ $<


#image: image.hex
#	objcopy -I ihex $< -O binary image.bin
#	hexdump -v -e '/1 "0x%02x, "' image.bin; echo ""
#	z80dasm -at image.bin > image.bin.lst
#
#image.hex: main.rel crt0.o 
#	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -o $@ main.rel crt0.rel
#
#%.rel: %.c
#	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -c -o $@ $<
#
#%.o: %.asm
#	sdasz80 -l -o -s $<

dump-main:
	m68k-linux-gnu-objdump -D main.elf

clean:
	rm *.o *.bin

