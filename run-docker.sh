#!/bin/sh
docker build --platform linux/amd64 -t contiker .
docker run --privileged --group-add uucp \
--group-add 986 \
--sysctl net.ipv6.conf.all.disable_ipv6=0 \
--mount type=bind,source=$PWD,destination=/home/user/contiki-ng \
-e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix -v /dev/bus/usb:/dev/bus/usb \
-ti --rm contiker