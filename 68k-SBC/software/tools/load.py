
import time
import serial
import argparse

filename = '/dev/ttyUSB0'
baud = 9600

def main():
    parser = argparse.ArgumentParser(prog='load', description='Load a program over serial into Computie68k')
    parser.add_argument("filename")
    args = parser.parse_args()

    try:
        tty = serial.Serial(filename, baud)
    except serial.serialutil.SerialException as exc:
        print("serial error: " + str(exc))
        return

    print("Opening {}".format(args.filename))
    with open(args.filename, 'r') as f:
        while True:
            ch = f.read(1)
            if not ch:
                break
            time.sleep(0.005)
            tty.write(bytes(ch, 'ascii'))

    tty.close()

if __name__ == '__main__':
    main()

