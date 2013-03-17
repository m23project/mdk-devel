#!/usr/bin/python

import kudzu

devices = kudzu.probe (kudzu.CLASS_MOUSE,
                       kudzu.BUS_UNSPEC,
                       kudzu.PROBE_ONE);

for device in devices:
#    (dev, driver, desc) = device
    print device
