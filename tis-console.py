import RPi.GPIO as GPIO
from time import sleep

import pdb

clk=8
io=10
delay=0.1

GPIO.setmode(GPIO.BOARD)
GPIO.setup(clk, GPIO.OUT, initial=GPIO.LOW)

WAIT=0
WRITE=1
READ=2

def negotiate(goal):
    rgoal=WAIT
    #pdb.set_trace()
    GPIO.output(clk, GPIO.HIGH)
    if goal == WRITE:
        GPIO.setup(io, GPIO.OUT, initial=GPIO.LOW)
    sleep(delay)
    GPIO.output(clk, GPIO.LOW)
    if goal != WRITE:
        GPIO.setup(io, GPIO.IN)
        rgoal += WAIT if GPIO.input(io) else WRITE
    sleep(delay)

    GPIO.output(clk, GPIO.HIGH)
    if goal == READ:
        GPIO.setup(io, GPIO.OUT, initial=GPIO.LOW)
    sleep(delay)
    GPIO.output(clk, GPIO.LOW)
    if goal != READ:
        #pdb.set_trace()
        GPIO.setup(io, GPIO.IN)
        rgoal += WAIT if GPIO.input(io) else READ
    sleep(delay)
    return rgoal

def transfer(data=None):
    out=0
    for i in range(8):
        GPIO.output(clk, GPIO.HIGH)
        if data != None:
            if data & 1<<i:
                GPIO.setup(io, GPIO.IN)
            else:
                GPIO.setup(io, GPIO.OUT, initial=GPIO.LOW)
        sleep(delay)

        GPIO.output(clk, GPIO.LOW)
        if data == None:
            GPIO.setup(io, GPIO.IN)
            inp = GPIO.input(io)
            out |= inp << i
        sleep(delay)
    return out

inp = int(input("Number: "))
print negotiate(WRITE)
transfer(inp)
print "done sending"
print negotiate(READ)
print transfer()
