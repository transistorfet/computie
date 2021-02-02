
import time
import serial
import argparse

devicefile = '/dev/ttyUSB0'
#baud = 9600
baud = 38400

def main():
    parser = argparse.ArgumentParser(prog='load', description='Load a program over serial into Computie68k')
    parser.add_argument('filename')
    parser.add_argument('-d', '--device', nargs='?', default=devicefile, help='Use an alternate serial device')
    parser.add_argument('-l', '--limit', action='store_true', help='Limit the speed of transmission')
    args = parser.parse_args()

    try:
        tty = serial.Serial(args.device, baud, rtscts=True)
    except serial.serialutil.SerialException as exc:
        print("serial error: " + str(exc))
        return

    print("Opening {}".format(args.filename))
    with open(args.filename, 'r') as f:
        while True:
            ch = f.read(1)
            if not ch:
                break
            if args.limit:
                time.sleep(0.001)
            #time.sleep(0.05)
            tty.write(bytes(ch, 'ascii'))

    tty.close()

if __name__ == '__main__':
    main()

