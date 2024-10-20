#!/usr/bin/env python3

import re
import hal
import linuxcnc

h = hal.component("toolrpm")

h.newpin("max-rpm", hal.HAL_FLOAT, hal.HAL_OUT)
h.ready()

# create a connection to the status channel
s = linuxcnc.stat()
c = linuxcnc.command()

n = 0

try:
    while 1:
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


except KeyboardInterrupt:
    raise SystemExit


