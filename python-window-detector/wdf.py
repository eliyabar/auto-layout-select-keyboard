from win32gui import GetWindowText, GetForegroundWindow
import platform


class WindowDetectorFactory:
    def __init__(self):
        self.os_type = None
        self.window_detector = None
        print 'window detector started'
        self.os_typ = platform.system()
        print 'os is: ', self.os_typ
        if self.os_typ == 'Windows':
            self.window_detector = WindowsWindowDetector()
        elif self.os_typ == 'Darwin':
            self.window_detector = MacWindowDetector()
        elif self.os_typ == 'Linux':
            self.window_detector = MacWindowDetector()

    def get_window(self):
        return self.window_detector.get_window_name()


class WindowsWindowDetector:
    def __init__(self):
        print 'Windows - window detector started'

    @staticmethod
    def get_window_name():
        return GetWindowText(GetForegroundWindow()).split("-")[-1]


class MacWindowDetector:
    def __init__(self):
        # https://stackoverflow.com/questions/10266281/obtain-active-window-using-python
        print 'Mac - window detector started'

    @staticmethod
    def get_window_name():
        return 'Mac Test'
