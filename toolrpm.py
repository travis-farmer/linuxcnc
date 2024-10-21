#!/usr/bin/env python3

import hal
import linuxcnc
import signal
import time



h = hal.component("toolrpm")

h.newpin("max-rpm", hal.HAL_FLOAT, hal.HAL_OUT)
h.ready()

# create a connection to the status channel
s = linuxcnc.stat()
n = 0


try:
	while True:
		s.poll() # get values from the status channel
		current_tool = s.tool_in_spindle # get curent tool-number
		inifile = linuxcnc.ini("/home/travis/linuxcnc/configs/my_LinuxCNC_router/my_LinuxCNC_router.ini")
		if s.tool_in_spindle != 0: # a tool is loaded
			tool_max = inifile.find("TOOL_MAX", str(current_tool)) or "24000.0"
			n = float(tool_max)
		else:
			n = 24000.0
		# set hal pin
		h['max-rpm'] = n

except KeyboardInterrupt:
    raise SystemExit


