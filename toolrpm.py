#!/usr/bin/env python3

import hal
import linuxcnc
import signal
import time

class PeriodicTimer:
    def __init__(self, interval, handler):
        self.interval = interval
        self.handler = handler
        self.signal = signal.SIGALRM

    def start(self):
        signal.signal(self.signal, self.handle_timeout)
        signal.setitimer(signal.ITIMER_REAL, self.interval, self.interval)

    def handle_timeout(self, signum, frame):
        self.handler()

def get_val():
    s.poll() # get values from the status channel
    current_tool = s.tool_in_spindle # get curent tool-number
    inifile = linuxcnc.ini(s.ini_filename)

    if s.tool_in_spindle != 0: # a tool is loaded
        tool_max = inifile.find("TOOL_MAX", current_tool) or "24000.0"
        n = float(tool_max)
    else:
        n = 24000.0
    # set hal pin
    h['max-rpm'] = n

h = hal.component("toolrpm")

h.newpin("max-rpm", hal.HAL_FLOAT, hal.HAL_OUT)
h.ready()

# create a connection to the status channel
s = linuxcnc.stat()
c = linuxcnc.command()

n = 0

timer = PeriodicTimer(0.01, get_val())  # 2 seconds interval
timer.start()

try:
    while True:
        time.sleep(1)  # Do other work

except KeyboardInterrupt:
    raise SystemExit


