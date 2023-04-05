#include "Arduino.h"
/*
  Sine/Square Wave Generator
  Aykut Satici
  
  Generates a Sine Wave with specified frequency at 12-bit resolution of DAC output
  Cycles through values in the Waveforms.h lookup file, which are created from 
  a python script and copied in manually
  
*/

// include lookup table of sine values from waveforms.h
#include "Waveforms.h"

// Pin 13 has the LED on Teensy 3.1 - will use this pin also as the square wave TTL output
int ledpin = 13;
int pwmpin = 37;

volatile int i = 0;

FASTRUN void timer0_callback() {
    analogWrite(pwmpin, waveformsTable[i]);  // write the selected waveform on DAC
    
    if (i<256) {
        digitalWrite(ledpin, HIGH);
    }
    else {
        digitalWrite(ledpin, LOW);
    }
    i++;
    if (i==512)
        i=0;
    // delayMicroseconds(21); // to slow it down a bit if required 3 us to 16383 us
    delayMicroseconds(200); // to slow it down a bit if required 3 us to 16383 us
}

// the setup routine runs once when you press reset:
IntervalTimer timer0;
FASTRUN void setup() {
    analogWriteResolution(12);
    pinMode(pwmpin,OUTPUT);
    analogWrite(pwmpin,0);
    pinMode(ledpin, OUTPUT);
    
    timer0.begin(timer0_callback, 1.25);  
}


void loop() {
  
}
