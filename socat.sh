#!/bin/bash
socat /dev/ttyNET,b9600,raw,echo=0 TCP:10.156.86.144:7000

[Unit]

Description=SocatScript

After=default.target

[Service]

ExecStart=/PATH/TO/socat.sh
ExecStop=killall socat

[Install]

WantedBy=default.target
