#!/bin/bash

find /etc/rc* | grep m23 | xargs -n1 rm
find /etc/init.d/ | grep m23 | xargs -n1 rm
sed -i 's#^Acquire#//Acquire#g' /etc/apt/apt.conf.d/70debconf