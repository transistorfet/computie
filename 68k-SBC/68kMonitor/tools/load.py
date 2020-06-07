
import time
import serial

filename = '/dev/ttyUSB0'
baud = 9600

def main():
    try:
        tty = serial.Serial(filename, baud)
    except serial.serialutil.SerialException as exc:
        print("serial error: " + str(exc))
        return

    with open('kernel.load', 'r') as f:
        while True:
            ch = f.read(1)
            if not ch:
                break
            time.sleep(0.001)
            tty.write(bytes(ch, 'ascii'))

    tty.close()

if __name__ == '__main__':
    main()

