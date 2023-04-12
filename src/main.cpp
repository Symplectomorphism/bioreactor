/*
 *
 * @file main.cpp
 * @author Aykut C. Satici
 * @brief Sine/square wave generator and acceleration logger
 *
 *        Generates a sine wave with specified amplitude and frequency at 12-bit
 *        resolution through values in the Waveforms.h lookup file.
 *        Reads acceleration data from LSM6DSOX IMU over i2c protocol.
 *
 * @version 1.0
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 *
 */

#include "Arduino.h"
#include <Adafruit_LSM6DSOX.h>

// Include lookup table of sine values from Waveforms.h
#include "Waveforms.h"

#define LSM6DSOX_ADDRESS            0x6A
// #define LSM6DSOX_CTRL1_XL           0X10
#define LSM6DSOX_CTRL8_XL           0X17

// Pin 13 has the LED on Teensy 4.1:
// will use this pin also as the square wave TTL output
int ledpin = 13;
int pwmpin = 28;

volatile int i = 0;

Adafruit_LSM6DSOX sox;
// sensors_event_t accel, gyro, tmp;
float x, y, z;      // accel values are saved here.

FASTRUN void timer0_callback() {
    // Change the amplitude here.
    int ampl = (int)(0.613*waveformsTable[i]);
    analogWrite(pwmpin, ampl);
    
    if (i<256) {
        digitalWrite(ledpin, HIGH);
    }
    else {
        digitalWrite(ledpin, LOW);
    }
    i++;
    if (i==512)
        i=0;
}

FASTRUN void timer1_callback() {
     if (sox.accelerationAvailable())
         sox.readAcceleration(x, y, z);

      Serial.print(x);
      Serial.print('\t');
      Serial.print(y);
      Serial.print('\t');
      Serial.print(z);
      Serial.print('\n');
}

// The setup routine runs once when you press reset:
IntervalTimer timer0;
IntervalTimer timer1;

FASTRUN void setup() {
    // PWM carrier frequency is set to 36.6kHz.
    analogWriteFrequency(pwmpin, 36600);
    analogWriteResolution(12);
    pinMode(pwmpin,OUTPUT);
    analogWrite(pwmpin,0);
    pinMode(ledpin, OUTPUT);

    Serial.begin(115200);
    while (!Serial)
        delay(10);

    if (!sox.begin_I2C()) {
        while (1) {
          delay(10);
        }
    }
    sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);

    // sox.setAccelDataRate(LSM6DS_RATE_833_HZ);
    // Changes the data rate just as the above line does but also enables LPF2.
    // LPF2: secondary low-pass filter
    Wire.beginTransmission(LSM6DSOX_ADDRESS);
    Wire.write(LSM6DSOX_CTRL1_XL);
    Wire.write(0x82);
    if (Wire.endTransmission() != 0) {
      return;
    }

    // Changes LPF2 settings
    Wire.beginTransmission(LSM6DSOX_ADDRESS);
    Wire.write(LSM6DSOX_CTRL8_XL);
    Wire.write(0x21);
    if (Wire.endTransmission() != 0) {
      return;
    }

    // Communicate as fast as possible over i2c
    Wire.setClock(1000000UL);

    /* 
     * Change the frequency of oscillation here.
     *
     * The frequency of oscillation in Hz is given by (delay in microseconds)
     * 1e6/delay/512,
     *
     * Conversely, given the frequency, say 90Hz, find the delay (us) by
     * 1e6/freq/512
     * 
     * delay must be between 3us and 16383 us for Teensy 4.1.
     *
     */
    timer0.begin(timer0_callback, 19);  
    timer1.begin(timer1_callback, 1000);  
}

// All the work is done in timer callbacks. No work to be done in the loop().
void loop() {  }
