import serial
import serial.tools.list_ports
import time
from wdf import WindowDetectorFactory

BAUDRATE = 9600
ser = None
thread = None
keymap_names = {'chrome': '10', 'pycharm': '11', 'firefox': '12', 'visual studio code': '13'}
current_keymap = None
window_detector = None
import threading

connected = False


def main():
    init()
    while 1:
        if connected:
            current_window_name = window_detector.get_window()
            print current_window_name
            for win_name in keymap_names.keys():
                if win_name in current_window_name.lower():
                    keymap = keymap_names[win_name]
                    global current_keymap
                    if current_keymap != keymap:
                        # try catch
                        print 'writing ', win_name, ' keymap: ', keymap
                        try:
                            ser.write(keymap + '\n')
                            current_keymap = keymap
                        except:
                            print "Error - No serial connection! connect keyboard and try again"
                            find_keyboard()
                        # time.sleep(1)
            time.sleep(1)


def init():
    global window_detector
    window_detector = WindowDetectorFactory()
    find_keyboard()


def find_keyboard():
    global ser, connected
    while not connected:
        ports = list(serial.tools.list_ports.comports())
        arduino_port = None
        for p in ports:
            if 'Arduino' in p.description:
                arduino_port = p.device
                connected = True
                print 'connecting to port - ', arduino_port
        if arduino_port:
            ser = serial_connect(arduino_port)
            global thread
            thread = threading.Thread(target=read_from_port, args=(ser,))
            thread.start()
        else:
            print 'Error - Could not find Keyboard, please connect USB'
        time.sleep(4)


def serial_connect(port):
    return serial.Serial(port, BAUDRATE, timeout=2)



def read_from_port(ser):
    while True:
        read_val = ser.readline()
        print read_val


if __name__ == "__main__":
    main()
