#!/usr/bin/env python
#
# BBArduino4RC.py
# Copyright: Pierre Neyron <pierre.neyron@free.fr>
# License: GNU GPL v3
#
import pygame
import os
from select import select
from sys import stdin
import serial

precalibrated=True
ser = serial.Serial(port='/dev/ttyACM0', baudrate=9600)
ttypy = os.open("/dev/pts/5", os.O_RDWR)
ttyser = os.open("/dev/pts/3", os.O_RDWR)
mapping = [2,0,1,5,4,3] 

def myprint(str):
    #print str
    os.write(ttypy, str + "\n")

pygame.init()
pygame.joystick.init()
    
joystick_count = pygame.joystick.get_count()
if joystick_count < 0:
    exit(0)

joystick = pygame.joystick.Joystick(0)
joystick.init()
    
myprint("Joystick name: {}".format(joystick.get_name()))
        
axes = joystick.get_numaxes()
myprint("Number of axes: {}".format(axes))
        
axis = range(6)
axis_max = range(6)
axis_min = range(6)
val_new = range(6)
val_old = range(6)

pygame.event.get() 

if precalibrated:
    axis_max[0] = 0.721649169922
    axis_min[0] = -0.597961425781
    axis_max[1] = 0.721649169922
    axis_min[1] = -0.433013916016
    axis_max[2] = 0.432983398438
    axis_min[2] = -0.608276367188
    axis_max[3] = 0.536071777344
    axis_min[3] = -0.639190673828
    axis_max[4] = 0.896881103516
    axis_min[4] = -0.876312255859
    axis_max[5] = 0.917510986328
    axis_min[5] = -0.979400634766
else:
    key=''
    while key == '':
        pygame.event.get() 
        for i in range( axes ):
            axis[i] = joystick.get_axis(mapping[i])
        myprint("X1,Y1 = {:>6.3f}, {:>6.3f}".format(axis[0],axis[1]))
        myprint("X2,Y2 = {:>6.3f}, {:>6.3f}".format(axis[2],axis[3]))
        myprint("Z,T = {:>6.3f}, {:>6.3f}".format(axis[4],axis[5]))
        myprint("Hit 'enter' when you are in the MIN position for all axes")
        rlist, _, _ = select([stdin], [], [], 1)
        if rlist:
            key = stdin.readline(1)
    for i in range( axes ):
        axis_min[i] =  joystick.get_axis( mapping[i] )
    
    key=''
    while key == '':
        pygame.event.get() 
        for i in range( axes ):
            axis[i] = joystick.get_axis(mapping[i])
        myprint("X1,Y1 = {:>6.3f}, {:>6.3f}".format(axis[0],axis[1]))
        myprint("X2,Y2 = {:>6.3f}, {:>6.3f}".format(axis[2],axis[3]))
        myprint("Z,T = {:>6.3f}, {:>6.3f}".format(axis[4],axis[5]))
        myprint("Hit 'enter' when you are in the MAX position for all axes")
        rlist, _, _ = select([stdin], [], [], 1)
        if rlist:
            key = stdin.readline(1)
    for i in range( axes ):
        axis_max[i] =  joystick.get_axis( mapping[i] )
    
    myprint("="*80)
    for i in range(axes):
        myprint("   axis_min[{}] = {}".format(i, axis_min[i]))
        myprint("   axis_max[{}] = {}".format(i, axis_max[i]))

while True:
    pygame.event.get() 
    myprint("="*80)
    for i in range( axes ):
        axis[i] = joystick.get_axis( mapping[i] )
        if ((axis_max[i] - axis_min[i]) == 0):
            val_new[i] = 0;
        else:
            val_new[i] = int(round(32 * (axis[i] - axis_min[i]) / (axis_max[i] - axis_min[i])))
            val_new[i] = max(val_new[i], 0)
            val_new[i] = min(val_new[i], 31)
    myprint("X1,Y1 = {:>6.3f} < {:>6.3f} <{:>6.3f}, {:>6.3f} < {:>6.3f} < {:>6.3f}".format(axis_min[0],axis[0],axis_max[0],axis_min[1],axis[1],axis_max[1]))
    myprint("X2,Y2 = {:>6.3f} < {:>6.3f} <{:>6.3f}, {:>6.3f} < {:>6.3f} < {:>6.3f}".format(axis_min[2],axis[2],axis_max[2],axis_min[3],axis[3],axis_max[3]))
    myprint("Z,T = {:>6.3f} < {:>6.3f} <{:>6.3f}, {:>6.3f} < {:>6.3f} < {:>6.3f}".format(axis_min[4],axis[4],axis_max[4],axis_min[5],axis[5],axis_max[5]))
    myprint("X1,Y1 = {}, {}".format(val_new[0],val_new[1]))
    myprint("X2,Y2 = {}, {}".format(val_new[2],val_new[3]))
    myprint("Z,T = {}, {}".format(val_new[4],val_new[5]))
    for i in range(4):
        code = (i<<5) + val_new[i]
        myprint("{:3}: {:3d} ({:08b})".format(i, code, code))
        if (val_new[i] != val_old[i]):
            ser.write(chr(code))
    code = (4<<5) + ((val_new[5]>15)<<4) + val_new[4]/2
    myprint("{:3}: {:3d} ({:08b})".format("4+5", code, code))
    if (val_new[4] != val_old[4] or val_new[5] != val_old[5]):
        ser.write(chr(code))
    while (ser.inWaiting() > 0):
        os.write(ttyser, ser.read(ser.inWaiting()))
    for i in range( axes ):
        val_old[i] = val_new[i]
    pygame.time.wait(200)            

pygame.quit ()
