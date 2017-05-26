/*
 * NOTE: There is an issue with using more than 2 ultrasonic sensors at the same time.
 * A third or fourth sensor always returned 0 distance regardless of where it was plugged in;
 * we have replaced the sensors, wires, and even the Arduino Mega so the issue must be software related.
 * This may be due to using the pulseIn function. Due to lack of time, we were not able to test this theory.
 * However, this can be tested using the NewPing library in place of our Ultrasonic library.
 * http://playground.arduino.cc/Code/NewPing
 */

//#define DEBUG

// functions that only print when debugging
#ifdef DEBUG
#define DebugPrint(x) Serial.print(x)
#define DebugPrintln(x) Serial.println(x)
#else
#define DebugPrint(x)
#define DebugPrintln(x)
#endif

#include <Wire.h>
#include <HCSR04Ultrasonic.h>
#include <ArdulinkMessageReceiver.h>

ArdulinkMessageReceiver Receiver;
// boolean to set motors to off, to avoid gradual roll
bool motorOff;

#include "Locomotion.h"
#include "SensorResponse.h"

boolean handleCustomMessage(String message);

void keyPressStub(char key) {}

// timer for ultrasonic
// prevents overlap between pulses
unsigned long const interval = 70;
unsigned long previousMillis = 0;

void setup() {
  // initialize Ardulink
  #ifndef DEBUG
    // set motors to off until a message is received
    motorOff = true;
    Receiver.init(handleCustomMessage, keyPressStub);
  #else
    motorOff = false;
    Serial.begin(9600);
  #endif
  // initialize sensors
  frontSensor.init();
  sideSensor.init();

  InitializeMotors();

  // change PWM frequency to 4000 Hz on timer 2
  // this is needed because the motors on the robot require a higher pwm frequency than
  // the default pwm frequency that the arduino mega can produce
  int prescalerval = 0x07; // used as an 'eraser'
  TCCR2B &= ~prescalerval; // sets first 3 bits from right to 0 using the 'eraser'
  prescalerval = 2; // used to set timer to 4000 Hz
  TCCR2B |= prescalerval; // sets the timer to 4000 Hz

  // boolean used to detect if there is an obstacle in the robots path
  // set to true initially so that robot begins in a stopped state
  frontStop = true;
  DebugPrintln("BEGIN");
}

void loop() {
  #ifndef DEBUG
    Receiver.processInput();
    if (motorOff)
      return;
  #endif

  unsigned long currentMillis = millis();
 
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    int inchesLeft = sideSensor.getDistanceIn();
    WallFollow(inchesLeft); 
  }
  StopIfPathObstructed();
  
}

////////////////Ardulink////////////////////
#ifndef DEBUG
void serialEvent() {
  Receiver.getInput();
}
#endif

boolean handleCustomMessage(String message) {
  motorOff = false;
  if (message.equals(String("YES"))) {
    userDisable();
    motorOff = true;
    return true;
  }
  else if (message.equals(String("NO"))) {
    userEnable();
    return true;
  }
  else {
    return false;
  }
}
///////////////////////////////////////
