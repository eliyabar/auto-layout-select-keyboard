from win32gui import GetWindowText, GetForegroundWindow
import time
while 1:
    time.sleep(1)
    print(GetWindowText(GetForegroundWindow()))
# https://stackoverflow.com/questions/10266281/obtain-active-window-using-python
