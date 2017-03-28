/*
 * Code to move big bot using libraries using keyboard
 */


#include <Wire.h>

// left motor
int analogPinLeft = 9; //analogPinLeft
int digitalPinLeft = 22; //digitalPinLeft

// right motor
int analogPinRight = 10; //analogPinRight
int digitalPinRight = 23; //digitalPinRight

// ultrasonic pins
int trigPin1 = 52;
int echoPin1 = 53;

// variables for ultrasonic ranging
long duration1, duration2, cm1, cm2, inches1, inches2;

// timer for ultrasonic
unsigned long const interval = 70;
unsigned long previousMillis = 0;

// boolean to set autonomous or user control
bool control;

bool motorOff;

int PWMValRight = 128;
int PWMValLeft = 128;

void setup() {
  Serial.begin(9600);

  //ultrasonic pins
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);

  //set pins to ground
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);

  // motor control
  pinMode(digitalPinLeft, OUTPUT);
  pinMode(digitalPinRight, OUTPUT);

  // change PWM frequency
  int prescalerval = 0x07;
  TCCR2B &= ~prescalerval;
  prescalerval = 2;
  TCCR2B |= prescalerval;

  // initial motor values
  digitalWrite(digitalPinLeft, LOW);
  digitalWrite(digitalPinRight, LOW);
  analogWrite(analogPinLeft, 128);
  analogWrite(analogPinRight, 128);

  // set control boolean to user
  control = true;
}

void loop() {
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
  
    // convert the time into a distance
    cm1 = (duration1/2) / 29.1;
    inches1 = (duration1/2) / 74;
  
    Serial.print(inches1);
    Serial.print("in, ");
    Serial.print(cm1);
    Serial.print("cm");
    Serial.println();
  }
  // user control
  if (Serial.available() > 0) {
    int input = Serial.read();
    switch(input) {
      case 'w': //both motors forward
                PWMValRight += 10;
                PWMValLeft += 10;
                updateSpeed();
                break;
      case 's': //both motors reverse
                PWMValRight -= 10;
                PWMValLeft -= 10;
                updateSpeed();
                break;
      case 'R': //right motor forward
                PWMValRight += 10;
                updateSpeed();
                break;
      case 'r': //right motor reverse
                PWMValRight -= 10;
                updateSpeed();
                break;
      case 'L': //left motor forward
                PWMValLeft += 10;
                updateSpeed();
                break;
      case 'l': //left motor reverse
                PWMValLeft -= 10;
                updateSpeed();
                break;
      case 'm': PWMValLeft = 220;
                PWMValRight = 220;
                updateSpeed();
                break;
      case 'n': PWMValLeft = 36;
                PWMValRight = 36;
                updateSpeed();
                break;
      case 'p': PWMValLeft = 36;
                PWMValRight = 220;
                updateSpeed();
                break;
      case 'o': PWMValLeft = 220;
                PWMValRight = 36;
                updateSpeed();
                break;
      case 'a': control = false;
                Serial.println("\n------- Autonomous Control -------\n");
                break;
      case 'u': control = true;
                Serial.println("\n---------- User Control ----------\n");
                break;
      case '0': userDisable();
                motorOff = true;
                Serial.println("\nDisable Motors\n");
                break;
      case '1': userEnable();
                Serial.println("\nEnable Motors\n");
                break;
      default:  //all stop
                PWMValLeft = 127;
                PWMValRight = 127;
                updateSpeed();
                break;
    }
  }

  if (!control) {
    updateSpeed();
  }

  if(inches1 <= 24) {
    userDisable();
  }
  else {
    if (!motorOff) {
      userEnable();
    }
    else {
      userDisable();
    }
  }
}

void updateSpeed() {
  // user control
  if (control) {
    boundsCheck();
    setMotorSpeed();
    printSpeeds();
  }
  // autonomous
  else {
    PWMValLeft = 220;
    PWMValRight = 220;
    setMotorSpeed();
  }
}

void setMotorSpeed() {
  digitalWrite(digitalPinRight, LOW);
  analogWrite(analogPinRight, PWMValRight);
  digitalWrite(digitalPinLeft, LOW);
  analogWrite(analogPinLeft, PWMValLeft);
}

void printSpeeds() {
  Serial.print("Right Motor: ");
  Serial.println(PWMValRight);
  Serial.print("\tLeft Motor: ");
  Serial.println(PWMValLeft);
}

void boundsCheck() {
  if (PWMValRight > 248) PWMValRight = 248;
  
  if (PWMValLeft > 248) PWMValLeft = 248;
  
  if (PWMValRight < 8) PWMValRight = 8;
  
  if (PWMValLeft < 8) PWMValLeft = 8;
}

void userDisable() {
  digitalWrite(digitalPinRight, HIGH);
  digitalWrite(digitalPinLeft, HIGH);
}

void userEnable() {
  digitalWrite(digitalPinRight, LOW);
  digitalWrite(digitalPinLeft, LOW);
}
