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

  // change PWM frequency
  int prescalerval = 0x07;
  TCCR2B &= ~prescalerval;
  prescalerval = 2;
  TCCR2B |= prescalerval;

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
