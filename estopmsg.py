#!/usr/bin/env python3

import re
import hal
import linuxcnc

h = hal.component("estopmsg")

h.newpin("toolsensor", hal.HAL_BIT, hal.HAL_IN)
h.newpin("hardware", hal.HAL_BIT, hal.HAL_IN)

h.ready()

# create a connection to the status channel
c = linuxcnc.command()
tsfired = False
hrdwrfired = False

try:
    while 1:
        if h['toolsensor'] == False and tsfired == False:
            tsfired = True
            c.text_msg("ESTOP: Tool Sensor!")
        elif h['toolsensor'] == True and tsfired == True:
            tsfired = False
        
        if h['hardware'] == False and hrdwrfired == False:
            hrdwrfired = True
            c.text_msg("ESTOP: Hardware loop!")
        elif h['hardware'] == True and hrdwrfired == True:
            hrdwrfired = False

except KeyboardInterrupt:
    raise SystemExit


