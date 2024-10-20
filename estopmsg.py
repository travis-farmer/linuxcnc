#!/usr/bin/env python3

import re
import hal
import linuxcnc

h = hal.component("msgestop")

h.newpin("toolsensor", hal.HAL_BIT, hal.HAL_IN)
h.newpin("hardware", hal.HAL_BIT, hal.HAL_IN)

h.ready()

# create a connection to the status channel
c = linuxcnc.command()
tsfired = False
hrdwrfired = False

try:
    while 1:
        if h['toolsensor'] == True and tsfired == False:
            tsfired = True
            c.text_msg("ESTOP: Tool Sensor!")
        elif h['toolsensor'] == False and tsfired == True:
            tsfired = False
        
        if h['hardware'] == True and hrdwrfired == False:
            hrdwrfired = True
            c.text_msg("ESTOP: Hardware loop!")
        elif h['hardware'] == False and hrdwrfired == True:
            hrdwrfired = False

except KeyboardInterrupt:
    raise SystemExit


