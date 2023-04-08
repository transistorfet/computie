
Compiling and Running Software
==============================

Building the Boards
-------------------

The gerber files for each revision are in the hardware directory for each board under `revisions/`,
along with an Errata file that might be useful to read.  Only 68k-SBC Rev.2, 68k-SMT Rev.1, and
k30-SBC Rev.2 should be built.  The earlier revisions have known issues that make the boards
unusable.


Initializing A 68k-* Board
--------------------------

For the boards with removable flash chips, they can be programmed externally using a generic chip
programmer, but I usually use an Arduino Mega with the 68k Supervisor shield, and ribbon cables to
connect it to the board.  Using the 68k Supervisor sketch in `software/arduino/68kSupervisor`, and
compiling the monitor.bin target, the monitor can be written to the flash chips in-circuit.

It's also possible to use the supervisor to host the monitor directly, with the arduino intercepting
bus cycles, but that requires the clock speed to be 10MHz or lower, or else it won't have time to
stop the cycle and booting will fail.  The jumpers on the board also need to be set, so that the
flash memory is mapped into the region at `0x200000` instead of `0x000000`, and the DTACK needs to
be disabled so that the arduino can provided in when it eventually catches up.  Using the arduino
hosting is much slower than the flash, but can help diagnose problems.

To compile the monitor:
```sh
cd software/
make monitor.bin
```

This will create both a monitor.bin, and output.txt, which is a C-compatible copy of the binary that
is included by the Supervisor sketch.  Whatever code is in output.txt will be hosted at address 0,
or will be written to flash.

From the arduino serial monitor, to erase the flash, write the monitor image, and verify the
resulting memory, run the following commands:
```sh
erase
send
verify
```


Initializing A k30-SBC
----------------------

The k30 requires a modified Arduino shield with the A0, DS, and DSACK1 signals added to additional
Arduino I/O pins.  Only the lower 8-bits of the data bus are used, so the extended header on the
board can be left unconnected.  I will eventually make a new version of the shield PCB.  It also
requires the k30FlashWriter sketch instead, which can write the flash chip in memory, but does not
have the ability to debug or host anymore.  That feature doesn't work at 12MHz and the 68030
technically should have a >12.5MHz clock, according to the specs, so I removed it.

The process is otherwise the same as for the 68k-* boards


Connecting to the Monitor
-------------------------

To connect to the monitor, attach a TTL-level serial converter to the first serial port header and
connect with any serial terminal program.  I use a 5V FTDI-in-cable serial-to-USB adapter, which is
similar to [this one](https://www.sparkfun.com/products/9718), and I use `miniterm` on my desktop,
which is provided by the `python3-serial` package.

```sh
pyserial-miniterm /dev/ttyUSB0 38400
```

If it works, you should see it print the welcome message, and the prompt, after pressing the reset
button.  The prompt will appear every time you press enter in the terminal.
```
Welcome to the 68k Monitor!

>
```

Loading Over Serial
-------------------

Once the board has been initialized and can boot the monitor from flash, it's possible to send
programs over the serial port.  The binaries can be compiled using the `.load` extension as the
target, which will generate a text file that contains the load command, the length of data, and the
data encoded as hexadecimal in ASCII.  It can be sent over serial with the python script in
`tools/`, while the terminal is open.  This allows you to see the command start.  The data itself
will not be echoed back to the terminal so it will appear to hang until the file has been
transferred.

On the desktop:
```sh
make monitor.load
python3 tools/load.py monitor.load
```

There is an option to `load.py` to limit the speed of transfer using `-l`, which is necessary if the
CTS/DTR signals are not working properly.

Once the file is complete, the load script will usually include a boot command that will
automatically jump to the location where the file was loaded.  You can check if the monitor is
running in RAM at `0x100000` (the default load location) or Flash memory at `0x000000` by using the
`info` command to see where the PC is pointing to.


Updating the Monitor
--------------------

It's possible to update the monitor in memory, by loading a new copy of the monitor over serial into
RAM and running it.  If the monitor is running in RAM, it's (semi)-safe to erase the flash, and
write the copy of the monitor in RAM into Flash.  This can render the board unbootable, but the
Supervisor or external flash writer can be used to fix that.

To erase the monitor in Flash and write a new copy, in the monitor running in RAM run the following:
```sh
eraserom
writerom
verifyrom
```


Running the Kernel
------------------

To compile the kernel, and compile the commands, run:
```sh
make kernel.load
```

It can be sent over serial like the monitor.  You might want to modify the kernel configuration in
`src/kernel/main.c` if you're using a board without the CompactFlash card, or you don't want to use
networking.


Building A FlashCard Root Image
-------------------------------

The image can be built using a loopback device and following make commands:
```sh
make create-image
make mount-image
make build-system
make umount-image
```
Which will produce a 20MB disk image in `minix-build.img` using the `build/` directory as the
mountpoint, and the minix1 filesystem.  It does not include a partition table, so that needs to be
created on the flash drive by other means.  It can be written to the partition-specific block device
file using `dd` on a unix-like desktop.

In order to boot off the CompactFlash directly, the `boot.load` script can be loaded over serial and
written to an alternate location such as `0x020000`.  It must be a location that is outside of the
Flash chip's sector in which monitor is written, or else the system will be unbootable.  The
boot.bin image doesn't contain a vector table like the monitor.bin image does.

After sending the `boot.load` file over serial, without running it, in the monitor, run the
following:
```sh
eraserom 20000
writerom 20000
verifyrom 20000
```

You can then boot from the monitor at any time by running:
```sh
boot 20000
```

The bootloader will print a period character for every 1K of the kernel image loaded from the disk
into RAM, and then it will run the kernel after.  If it works, you should see the heartbeat LED
flashing to indicate the kernel is running and multiprocessing is enabled and switching.


Configuring Networking
----------------------

If the second serial port is connected to a desktop, it will act like a SLIP connection, which can
be bridged to the desktop's network, where `/dev/ttyUSB1` is the serial device connected to the
second serial port of the board, and `192.168.1.x` is the local network, and enp3s0 is the
internet-connected network interface on the linux desktop:

```sh
sudo slattach -s 38400 -p slip /dev/ttyUSB1
sudo ifconfig sl0 192.168.1.2 pointopoint 192.168.1.200 up
# (this is automatically added on my machine, but might be required) sudo route add -host 192.168.1.200 sl0
sudo arp -Ds 192.168.1.200 enp3s0 pub
sudo iptables -A FORWARD -i sl0 -j ACCEPT
sudo iptables -A FORWARD -o sl0 -j ACCEPT
sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
```

The device will have IP `192.168.1.200` which also needs to be configured in the kernel's
`src/kernel/main.c` function at the bottom.

