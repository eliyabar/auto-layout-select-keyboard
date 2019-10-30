# Auto layout select keyboard
This is an Arduino mechanical macro keyboard project.
The idea about this keyboard is that it comes with a program that changes the layout according to the current window.
for example:
consider a "next breakpoint" button B1 on the keyboard. It is possible to configure it differently for some detected windows.
* const uint8_t nextBreakPointIntellij[] = {KEY_F9};
* const uint8_t continueVSCode[] = {KEY_F5};
if intellij window is being used, the B1 key will be set for F9 key.
but if VScode window is the currently used window, it will use F5 as B1 key.
This allows us to use single functionality across different programs/operating systems.

I created the code from scratch, I'm sure that there are better ways to improve it.

CPK.ino is the arduino code for the keyboard.
python-window-detector - is the python code for window detecting. right now only windows is supported.

To build the keyboard i used matrix layout (Rows and Cols) with simple mechanical switches, but then after my first POC, moved to Sweet 16 PCB and it works great.

python-window-detector - this python app should pick up the name of the currently used window, check if the name is within the supported layout dictionary and if it is, it will send the layout code via serial communication to the arduino, then the arduino will change its layout.
