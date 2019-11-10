
The Z80 Not Quite Computer
==========================

A simple platform for experimenting with Z80 CPUs.  The Z80Proto board has a Z80 CPU, with a 32KB SRAM chip, a 32K EEPROM
chip, along with bus of pins for connecting to other things.  It does not have any onboard I/O, or an onboard power supply
(hence why it's not quite a computer).  The intention is to connect the bus pins to an Arduino Mega, which acts as a supervisor,
controlling the CPU through the BUSREQ and RESET lines, and emulating I/O devices.  It also has a simple clock circuit that
can optionally be used.  It was made partly to try out some 3rd party PCB manufacturers and partly because of signal issues
when breadboarding circuits with so many wires, which caused memory read errors.

Z80Supervisor is an arduino program which can run or stop the CPU, and read and write to memory directly (when the CPU is
suspended).  It is also able to respond to memory bus requests from the CPU, as long as the CPU is running with a slow clock.
It can both act as a ROM, serving up a Z80 program, and act as an I/O device by forwarding bytes on to the arduino's serial
port.

Z80Monitor contains programs that run on the Z80, which can either be loaded into the EEPROM, or served by the supervisor.

