

all: rom


rom: welcome.bin
	hexdump -v -e '/1 "0x%02x, "' $<; echo ""

memtest: memtest.bin
	hexdump -v -e '/1 "0x%02x, "' $<; echo ""

%.bin: %.asm
	z80asm -o $@ $<


image: image.hex
	objcopy -I ihex $< -O binary image.bin
	#hexdump -v -e '/1 "0x%02x, "' image.bin; echo ""
	hexdump -v -e '/1 "0x%02x, "' image.bin > output.txt
	z80dasm -at image.bin > image.bin.lst

image.hex: main.rel crt0.o 
	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -o $@ main.rel crt0.rel

%.rel: %.c
	sdcc -mz80 --no-std-crt0 --code-loc 0x0100 --data-loc 0x800 --stack-loc 0x0C00 -c -o $@ $<

%.o: %.asm
	sdasz80 -l -o -s $<

clean:
	rm *.o *.sym *.lst *.rel *.lk *.map

