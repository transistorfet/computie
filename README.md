
The 68k Single Board Computer - Rev 0
=====================================

A simple single board computer using a Motorola 68010 CPU, which can connect to a host computer via USB/TTL-Serial.
It has a bus for connecting a supervisor arduino or expansion devices, but it can operate standalone using the onboard
UARTs.  It has 64kB of onboard EEPROM, 1MB of onboard SRAM, and a 68681 Dual UART chip with GPIO.  Some of the GPIO
pins are wired to 4 general purpose LEDs and 2 general purpose buttons.

[68k Board Schematics](https://github.com/transistorfet/computie/raw/master/hardware/68k-SBC/68kBoard/68kBoard.pdf)


The serial ports require external TTL-to-something conversion and are designed to be used with an FTDI USB-to-Serial
cable or adapter.  Two optional jumpers can be used to supply power to the board from one of the USB ports (only one
should be jumped at a time, and only if the onboard power supply is disconnected), but it might be too much power to
source from USB, which is why it has the optional onboard power supply.

Two jumpers are provided to select what address the ROM will appear in (either 0x0000 or 0x2000), and whether to enable
onboard DTACK generation for address space 0x0000.  By moving the ROM to 0x2000 and disabling DTACK in 0x0000, a
connected arduino can boot the 68000 directly.

I decided to use discrete 74 Series chips for the glue logic to keep it simple and easy for others to build.  I tried
to minimize the amount of logic chips needed, so decided not to include interrupt support, which would have required at
least 2 more chips.  The interrupt acknowledge circuitry can be provided externally.

Breadboard Version
------------------

![alt text](https://github.com/transistorfet/computie/raw/master/images/Breadboard-serial.jpg "68k-SBC Breadboard")

Revision 0
----------

![alt text](https://github.com/transistorfet/computie/raw/master/images/PCB-rev0-front.jpg "68k-SBC PCB Front")
![alt text](https://github.com/transistorfet/computie/raw/master/images/SBC-rev0.jpg "68k-SBC")
![alt text](https://github.com/transistorfet/computie/raw/master/images/PCB-rev0-patch.jpg "68k-SBC patch for design mistake")

I made a mistake and left out the logic for LDS/UDS in the chip select for the high and low memory chips.  When writing a
single byte to memory, it would also attempt to write to both the high and low chips, instead of only one.  Thanks to a poster
on EEVblog who suggested patching it with a P-channel MOSFET shown soldered to the back of the SRAM chips to interpose the UDS/LDS
signals into the chip selects, with the original traces cut.

