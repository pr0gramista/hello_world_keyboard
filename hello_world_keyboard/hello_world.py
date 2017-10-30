import keyboard
import serial
import re
import time
import os

check_regex = re.compile('(press|release),([0-9]*)')
port = 'COM3'
rate = 9600

def press(code):
    print('press {}'.format(code))

    if code == 0:
        keyboard.press_and_release("alt+f4")
    elif code == 1:
        os.system("explorer")
    elif code == 2:
        os.system('"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe"')
    elif code == 3:
        keyboard.write('poprosturonin@gmail.com')


def release(code):
    print('release {}'.format(code))


def process(port, rate):
    s = serial.Serial(port, rate, timeout=0, parity=serial.PARITY_EVEN, rtscts=1)
    print("Connected to keyboard")
    while True:
        r = s.read_all()
        if len(r) > 0:
            reads = r.decode("utf-8").rstrip('\r\n').split('\r\n')
            for read in reads:
                match = check_regex.match(read)
                if match:
                    action, code = read.split(',')
                    code = int(code)

                    if action == 'press':
                        press(code)
                    elif action == 'release':
                        release(code)

while True:
    try:
        process(port, rate)
    except serial.SerialException:
        print("Keyboard disconnected, connection retry in 5 seconds")
    time.sleep(5)