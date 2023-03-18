#!/usr/bin/env python3
from serial import Serial as Serial
from time import sleep as sleep
from struct import unpack as unpack
import sys
import signal

from serial.serialutil import SerialException

def end_handler(sig, frame):
    print(energy)
    print(power)
    exit()
signal.signal(signal.SIGTERM, end_handler)
device="/dev/ttyACM0"
interval=.019
ser=Serial(device) 
energy=0
power=0
prev_power=0
while(True):      
    #print("here")
    ser.write(b"Get Meter Data")
    sleep(interval)
    
    try:
        power=unpack("f", ser.read(20)[8:12])[0]
        """voltage = unpack("f", ser.read(4))[0]
        current = unpack("f", ser.read(4))[0]
        power = unpack("f", ser.read(4))[0]
        voltageDP = unpack("f", ser.read(4))[0]
        voltageDM = unpack("f", ser.read(4))[0]"""
        #print(power)
    except SerialException:
        print("power meter error")
        exit()
    #if(prev_power==power):
    #    eq+=1
    #else:
    #   print(eq)
    #    eq=0
    if(prev_power>0):
        energy+=(((power+prev_power)/2)*20)/3600
    prev_power=power
    #energy+=power*.02
        
    
    #out+=str(power)+"\n"
    #print(str(power)+" "+str(voltage) +" "+ str(current)+" "+str(voltage*current))
    #print(power)
   