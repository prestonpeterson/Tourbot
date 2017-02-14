/*
 * created by Rui Santos, http://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */

#include <Wire.h>
#include "SeeedOLED.h"
 
int trigPin1 = 2;    //Trig - green Jumper
int echoPin1 = 56;    //Echo - yellow Jumper

int trigPin2 = 3;    //Trig - 
int echoPin2 = 57;    //Echo - 

// communicates with 4wd robot controller
int motorPinLowBit = 42;
int motorPinHighBit = 44;

long duration1, duration2, cm1, cm2, inches1, inches2;

// boolean for if robot is moving
bool moving;

// timer for ultrasonic
unsigned long const interval = 70;
unsigned long previousMillis = 0;

// timer for signals sent
unsigned long const interval2 = 3000;
unsigned long previousMillis2 = 0;
 
void setup() {
  //Serial Port begin
  Serial.begin (9600);
  //Define inputs and outputs
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(motorPinLowBit, OUTPUT);
  pinMode(motorPinHighBit, OUTPUT);

  Wire.begin();
  SeeedOled.init();  //initialze SEEED OLED display

  SeeedOled.clearDisplay();          //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();           //Set addressing mode to Page Mode
  SeeedOled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column
}
 
void loop()
{
 // digitalWrite(motorPin, HIGH);
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin1, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin1, LOW);
   
    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    pinMode(echoPin1, INPUT);
    duration1 = pulseIn(echoPin1, HIGH);

    digitalWrite(trigPin2, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);

    pinMode(echoPin2, INPUT);
    duration2 = pulseIn(echoPin2, HIGH);
   
    // convert the time into a distance
    cm1 = (duration1/2) / 29.1;
    inches1 = (duration1/2) / 74;

    cm2 = (duration2/2) / 29.1;
    inches2 = (duration2/2) / 74;

    SeeedOled.setTextXY(0,0);
    SeeedOled.putString("Front Sensor:");
    SeeedOled.setTextXY(1,4);
    SeeedOled.putNumber(inches1);
    SeeedOled.putString("in, ");
    //SeeedOled.putNumber(cm1);
    //SeeedOled.putString("cm\n");

    SeeedOled.setTextXY(2,0);
    SeeedOled.putString("45 Deg Sensor: ");
    SeeedOled.setTextXY(3,4);
    SeeedOled.putNumber(inches2);
    SeeedOled.putString("in, ");
    //SeeedOled.putNumber(cm2);
    //SeeedOled.putString("cm\n");

/*    
    Serial.print(inches1);
    Serial.print("in, ");
    Serial.print(cm1);
    Serial.print("cm");
    Serial.println(); 
    
    Serial.print(inches2);
    Serial.print("in, ");
    Serial.print(cm2);
    Serial.print("cm");
    Serial.println();
*/   

    if ( inches1 > inches2 ) {
      inches1 = inches2;
    }
  }
  // all stop
  if(inches1 <= 18 && moving==true) {
    digitalWrite(motorPinLowBit, LOW);
    digitalWrite(motorPinHighBit, LOW);
    Serial.print("Stop");
    moving = false;
    delay(1000);
  }
  // turn
  else if (inches1 <= 18 && moving==false) {
    digitalWrite(motorPinLowBit, LOW);
    digitalWrite(motorPinHighBit, HIGH);
    Serial.print("Turn");
    previousMillis2 = currentMillis;
  }
  // forward slowly
  else if (inches1 > 18 && moving==false){
    digitalWrite(motorPinLowBit, HIGH);
    digitalWrite(motorPinHighBit, LOW);
    Serial.print("Forward slowly");
    if (currentMillis - previousMillis2 >= interval2) {
      previousMillis2 = currentMillis;
      moving = true;
    }
  }
  // forward full speed
  else {
    digitalWrite(motorPinLowBit, HIGH);
    digitalWrite(motorPinHighBit, HIGH);
    Serial.print("Full speed ahead");
    moving = true;
  }
}
