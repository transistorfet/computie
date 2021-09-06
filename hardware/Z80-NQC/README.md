
The Z80 Not Quite Computer
==========================

A simple platform for experimenting with Z80 CPUs.  The Z80Proto board has a Z80 CPU, with a 32KB SRAM chip and a 32KB EEPROM
chip, along with a bus of pins for connecting to other things.  It also has a simple clock circuit that can optionally be used.
It does not have any onboard I/O, or an onboard power supply (hence why it's not quite a computer).  The intention is to connect
the bus pins to an Arduino Mega, which acts as a supervisor, controlling the CPU through the BUSREQ and RESET lines, and
emulating I/O devices.  It was made partly to try out some 3rd party PCB manufacturers and partly because of signal issues when
breadboarding circuits with so many wires, which was causing memory read errors.

Z80Supervisor is an arduino program which can run or stop the CPU, and read and write to memory directly (when the CPU is
suspended).  It is also able to respond to memory bus requests from the CPU, as long as the CPU is running with a slow clock.
It can both act as a ROM, serving up a Z80 program, and act as an I/O device by forwarding bytes on to the arduino's serial
port.

Z80Monitor contains programs that run on the Z80, which can either be loaded into the EEPROM, or served by the supervisor.


Schematic
---------

![alt text](https://github.com/transistorfet/computie/raw/main/Z80-NQC/Z80Proto/Z80Proto.png "Z80 NQC Proto Board")


Board
-----

![alt text](https://github.com/transistorfet/computie/raw/main/Z80-NQC/images/Z80-NCQ-Built.jpg "Z80 NQC Proto Board")
![alt text](https://github.com/transistorfet/computie/raw/main/Z80-NQC/images/Z80-NCQ-Board-Front.jpg "Z80 NQC Proto Board")
![alt text](https://github.com/transistorfet/computie/raw/main/Z80-NQC/images/Z80-NCQ-Board-Back.jpg "Z80 NQC Proto Board")
