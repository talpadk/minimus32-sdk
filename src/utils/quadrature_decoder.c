#include "quadrature_decoder.h"


void quadrature_decoder_init(QuadratureDecoder *decoder){
  decoder->lastQuad = 0;
  if (decoder->aInput()){
    decoder->lastQuad += 1;
  }
  if (decoder->bInput()){
    decoder->lastQuad += 2;
  }
}

void quadrature_decoder_handleEvent(QuadratureDecoder *decoder){
  unsigned char newQuad = 0;
  unsigned char quadDiff;
  unsigned char a = decoder->aInput();
  unsigned char b = decoder->bInput();
  
  char direction = 1;
  
  if (decoder->aInput()){
    newQuad += 1;
  }
  if (decoder->bInput()){
    newQuad += 2;
  }

  quadDiff = newQuad ^ decoder->lastQuad;
  decoder->lastQuad = newQuad;

  if (quadDiff == 1){
    //A changed
    if ((a && (!b)) ||
	((!a) && b)   ){ direction = -1; }
  }
  else if (quadDiff == 2){
    //B changed
    if ((b && a) ||
	((!b) && (!a))) { direction = -1; }
  }
  else {
    //Two changes == noice
    direction = 0;
  }
  if (direction!=0){
    if (direction == 1){ decoder->callback(1); }
    else { decoder->callback(0); }
  }
}


/*
#!/usr/bin/python
import RPi.GPIO as GPIO  
import time
import sys

PIN_BUTTON=3
PIN_A=5
PIN_B=7
DEBOUNCE=75

GPIO.setmode(GPIO.BOARD)
GPIO.setup(PIN_BUTTON, GPIO.IN)
GPIO.setup(PIN_A, GPIO.IN)
GPIO.setup(PIN_B, GPIO.IN)

index_=0
oldout=0

lastQuad_ = 0
if (GPIO.input(PIN_A)):
    lastQuad_ += 1
if (GPIO.input(PIN_B)):
    lastQuad_ += 2


def encoderCallback(pin):
    global index_
    global lastQuad_
    global PIN_A
    global PIN_B
    global DEBOUNCE
    global oldout

    aSignal = GPIO.input(PIN_A)
    bSignal = GPIO.input(PIN_B)

    newQuad = 0
    if (aSignal):
        newQuad += 1
    if (bSignal):
        newQuad += 2

    quadDiff = newQuad 1^ lastQuad_
    lastQuad_ = newQuad

    direction = -1

    if (quadDiff == 1):   #A Diff
        if ((aSignal and (not bSignal)) or        #A rising  and B Low
            ((not aSignal) and bSignal)):         #A falling and B High
            direction = 1
            #print "A changed"
    elif (quadDiff == 2): #B Diff
        if ((bSignal and aSignal) or              #B rising  and A High
            ((not bSignal) and (not aSignal))):   #B falling and A Low 
            direction = 1
            #print "B changed"
    else:                 #More than one signal changed at once, should never happen must be noise
        direction = 0
        #print "noise"

    index_ += direction
    if oldout != (index_/4):
        #print "e"+str(direction)
        sys.stdout.flush()
    print index_/4

    oldout=index_/4

time_stamp=0
button_state=GPIO.input(PIN_BUTTON)
button_active=False
def buttonCallback(pin):
    global time_stamp
    global button_state
    global button_active
    if button_active==False:
        button_active=True;
        time_now=time.time()
        time.sleep(0.05)
        if GPIO.input(pin) != button_state:
            button_state= GPIO.input(pin)
            print "b"+str(button_state)
            sys.stdout.flush()

        time_stamp=time_now
        button_active=False
    
GPIO.add_event_detect(PIN_A, GPIO.BOTH, callback=encoderCallback)
GPIO.add_event_detect(PIN_B, GPIO.BOTH, callback=encoderCallback)
GPIO.add_event_detect(PIN_BUTTON, GPIO.BOTH, callback=buttonCallback)

while True:
    time.sleep(1)
*/
