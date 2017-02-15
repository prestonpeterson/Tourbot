#include "motordriver_4wd.h"
#include "seeed_pwm.h"

//int ledPin = 13; // LED connected to digital pin 13
int lowPin = 2;
int highPin = 3;
int prevVal = 0;     // variable to store the read value

void setup()
{
  Serial.begin(9600);
//  pinMode(ledPin, OUTPUT);      // sets the digital pin 13 as output
  pinMode(lowPin, INPUT);      // sets the digital pin 7 as input
  pinMode(highPin, INPUT);
//  attachInterrupt(digitalPinToInterrupt(inPin), derp, LOW);
//  digitalWrite(inPin, HIGH);
  Serial.println("DERP");
  MOTOR.init();
}

void loop()
{
  Serial.print("DERP");
  int newVal = digitalRead(highPin) * 2 + digitalRead(lowPin);   // read the input pins
  Serial.println(newVal);

  if(prevVal != newVal) {
    prevVal = newVal;
    switch(newVal) {
      case 0: // Full stop
        MOTOR.setSpeedDir1(0, DIRR);
        MOTOR.setSpeedDir2(0, DIRF);
        break;
      case 1: // Slow ahead
        MOTOR.setSpeedDir1(20, DIRR);
        MOTOR.setSpeedDir2(20, DIRF);
        break;
      case 2: // Turn Right
        MOTOR.setSpeedDir1(20, DIRR);
        MOTOR.setSpeedDir2(20, DIRR);
        break;
      case 3: // Forward
        MOTOR.setSpeedDir1(40, DIRR);
        MOTOR.setSpeedDir2(40, DIRF);
        break;
    }
  }
  
  delay(20);
}
