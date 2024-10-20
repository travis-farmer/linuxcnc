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
        if s.tool_in_spindle != 0: # a tool is loaded
            tool_w = s.tool_table[current_tool].woffset
            # c.text_msg(tool_w)
            if tool_w != 0:
                n = float(tool_w)
            else: # comment string does not contain the information
                n = 24.0
        else:
            n = 24.0
        # set hal pin
        h['max-rpm'] = n


except KeyboardInterrupt:
    raise SystemExit


