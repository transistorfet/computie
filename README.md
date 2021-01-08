
Computie68k
===========

A 68k-based computer, running a simple unix-like operating system.  68k-SBC is a single board computer with
a 68010 CPU, 1MB SRAM, 1MB Flash, and a 68681 Dual UART controller.  The OS is a simple monolithic kernel
written in C, with preemptive multitasking via the 68681 timer interrupt.

68k-SMT is a mostly identical design using SMT and PLCC components.  A CompactFlash card slot has been added
to this version, but it is also possible to connect a simple breadboarded version to 68k-SBC.


The Operating System
--------------------

Currently it doesn't have a name.  It's largely based on reading Operating Systems Design and Implementation 2ed by
Andrew S. Tanenbaum, although I've gone with a monolithic design, at least for the time being, for the simplicity of
it.  It can do preemptive multitasking using the 68681 timer.  It also has an implementation of the minix version 1
filesystem, which uses RAM (through a device driver) to store the data, or a Compact Flash card connected as an IDE
device.

![alt text](https://github.com/transistorfet/computie/raw/master/images/OS-basic.gif "OS")


The 68k Single Board Computer
-----------------------------

A simple single board computer using a Motorola 68010 CPU, which can connect to a host computer via USB/TTL-Serial.
It has a bus for connecting a supervisor arduino or expansion devices, but it can operate standalone using the onboard
UARTs.  It has 1MB of onboard Flash, 1MB of onboard SRAM, and a 68681 Dual UART chip with GPIO.  Some of the GPIO
pins are wired to 4 general purpose LEDs and 2 general purpose buttons.

[68k Board Schematics](https://github.com/transistorfet/computie/raw/master/hardware/68k-SBC/68kBoard.pdf)


The serial ports require external TTL-to-something conversion and are designed to be used with an FTDI USB-to-Serial
cable or adapter.  Two optional jumpers can be used to supply power to the board from one of the USB ports (only one
should be jumped at a time, and only if the onboard power supply is disconnected).  It typically takes about 365mA at
5V to power.

Two jumpers are provided to select what address the ROM will appear in (either 0x0000 or 0x2000), and whether to enable
onboard DTACK generation for address space 0x0000.  By moving the ROM to 0x2000 and disabling DTACK in 0x0000, a
connected arduino can boot the 68000 directly.

I decided to use discrete 74 Series chips for the glue logic to keep it simple and easy for others to build.  I tried
to minimize the amount of logic chips needed, so it doesn't have a watchdog timer to trigger a bus error when an
unmapped address is accessed.

It's currently clocked at 12MHz, although there seems to be an issue with the arduino's DTACK generation at that speed.
It previously was working at 10MHz on the rev0 board.

The SBC version uses all DIP packaged chips and through-hole parts.  The SMT version uses surface mount chips for the
logic and ram, and PLCC packaged chips for the rest.  A CompactFlash socket and a 74HC32 to accommodate it has been
added and the power supply has been removed, but it is otherwise the same design.

SMT Revision 1
--------------

![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SMT-rev1/SMT.jpg "68k-SMT Rev. 1")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SMT-rev1/SMT-running.jpg "68k-SMT Running")

SBC Revision 2
--------------

![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev2/SBC.jpg "68k-SBC Rev. 2")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev2/ArduinoShield.jpg "Supervisor Arduino Shield")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev2/SBC-running.jpg "68k-SBC Running")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev2/PCBs.jpg "68k-SBC PCBs")


SBC Revision 0
--------------

This version has 64KB of EEPROM instead of Flash, and doesn't have onboard interrupt circuitry.  It started off with a
68000 CPU, but that was later upgraded to a 68010 to allow for relocatable interrupt tables when using off-board
interrupt circuitry.

![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev0/PCB-front.jpg "68k-SBC PCB Front")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev0/SBC.jpg "68k-SBC")
![alt text](https://github.com/transistorfet/computie/raw/master/images/68k-SBC-rev0/PCB-patch.jpg "68k-SBC patch for design mistake")

I made a mistake and left out the logic for LDS/UDS in the chip select for the high and low memory chips.  When writing a
single byte to memory, it would also attempt to write to both the high and low chips, instead of only one.  Thanks to a poster
on EEVblog who suggested patching it with a P-channel MOSFET shown soldered to the back of the SRAM chips to interpose the UDS/LDS
signals into the chip selects, with the original traces cut.

Breadboard Version
------------------

![alt text](https://github.com/transistorfet/computie/raw/master/images/Breadboard-serial.jpg "68k-SBC Breadboard")

