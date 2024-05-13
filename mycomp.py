#!/usr/bin/env python3

import re
import hal
import linuxcnc

h = hal.component("mycomp")

h.newpin("number-of-teeth", hal.HAL_FLOAT, hal.HAL_OUT)
h.ready()

# create a connection to the status channel
s = linuxcnc.stat()
n = 0

try:
    while 1:
        s.poll() # get values from the status channel
        current_tool = s.tool_in_spindle # get curent tool-number
        if s.tool_in_spindle != 0: # a tool is loaded
            tool_comment = s.toolinfo(current_tool)['comment'] # get the comment string of the loaded tool
            m = re.search(r'(?<=n=)\d+', tool_comment) # search the comment string for a one- or two-digit number after 'n='
            if m is not None: # comment string contains the information
                n = float(m.group(0))
            else: # comment string does not contain the information
                n = 0
        else:
            n = 0
        # set hal pin
        h['number-of-teeth'] = n


except KeyboardInterrupt:
    raise SystemExit


