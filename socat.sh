#!/bin/bash
socat /dev/ttyS2,b115200,raw,echo=0 TCP:192.168.123.30:7000
