#!/usr/bin/python

import serial
import sys
import struct
import time

def internet_time():
    "Swatch Internet Time. Biel meridian."
    h, m, s = time.gmtime()[3:6]
    h += 1 # Biel time zone: UTC+1
    seconds = s + (60.0*m) + (60.0*60.0*h)
    beats = seconds * 1000.0 / (60.0*60.0*24.0)
    beats = beats % 1000.0
    return beats

if __name__ == '__main__':
    beats=internet_time()
    bstr = "{0:05.0f}".format(beats*100)
    print "Beats: {0} -> {1}".format(beats,bstr)
    portname = '/dev/ttyACM0'
    if len(sys.argv) > 1: 
        portname = sys.argv[1]
    s = serial.Serial(portname, 15200, timeout=0.5)
    s.write(bstr+'\n')
    sys.stdout.write(s.readline())
    s.close()





