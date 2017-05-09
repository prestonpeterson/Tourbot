#define DEBUG
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
#include "Locomotion.h"
#include "SensorResponse.h"

ArdulinkMessageReceiver Receiver;

boolean handleCustomMessage(String message);

void keyPressStub(char key) {}

// timer for ultrasonic
unsigned long const interval = 70;
unsigned long previousMillis = 0;

// boolean to set motors to off, to avoid gradual roll
bool motorOff;

void setup() {
  // initialize Ardulink
  #ifndef DEBUG
    Receiver.init(handleCustomMessage, keyPressStub);
  #else
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

  // set motors to off
  motorOff = true;
  frontStop = true;
}

void loop() {
  #ifndef DEBUG
    Receiver.processInput();
  #endif

  unsigned long currentMillis = millis();
  int inchesLeft = 0;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    inchesLeft = frontSensor.getDistanceIn();
  }
  StopIfPathObstructed();
  WallFollow(inchesLeft); 
}

////////////////Ardulink////////////////////
void serialEvent() {
  Receiver.getInput();
}

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
