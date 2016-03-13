import RPi.GPIO as GPIO
from time import sleep

import pdb

clk=8
io1=10
io2=12
probe=16
delay=0.1

GPIO.setmode(GPIO.BOARD)
GPIO.setup(clk, GPIO.OUT, initial=GPIO.LOW)
GPIO.setup(probe, GPIO.IN)

WAIT=0
WRITE=1
READ=2

def negotiate(io, goal):
    rgoal=WAIT
    #pdb.set_trace()
    GPIO.output(clk, GPIO.HIGH)
    if goal == WRITE:
        GPIO.setup(io, GPIO.OUT, initial=GPIO.LOW)
    sleep(delay)
    GPIO.output(clk, GPIO.LOW)
    inp=0
    if goal != WRITE:
        GPIO.setup(io, GPIO.IN)
        inp = GPIO.input(io)
        rgoal += WAIT if inp else WRITE
    #print inp, GPIO.input(probe)
    sleep(delay)

    GPIO.output(clk, GPIO.HIGH)
    if goal == READ:
        GPIO.setup(io, GPIO.OUT, initial=GPIO.LOW)
    sleep(delay)
    GPIO.output(clk, GPIO.LOW)
    if goal != READ:
        #pdb.set_trace()
        GPIO.setup(io, GPIO.IN)
        inp = GPIO.input(io)
        rgoal += WAIT if inp else READ
    #print inp, GPIO.input(probe)
    sleep(delay)
    return rgoal

def transfer(io, data=None):
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
        inp=0
        if data == None:
            GPIO.setup(io, GPIO.IN)
            inp = GPIO.input(io)
            out |= inp << i
        #print inp, GPIO.input(probe)
        sleep(delay)
    return out

inp = int(input("Number: "))
state = WAIT
while(state != READ):
    state = negotiate(io1, WRITE)
    #print state
    transfer(io1, inp)
print "done sending"
while(state != WRITE):
    state = negotiate(io2, READ)
    #print state
    val = transfer(io2)
    if state == WRITE:
        print "Result: ", val;
