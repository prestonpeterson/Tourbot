#include <Wire.h>
#include <HCSR04Ultrasonic.h>
#include <ArdulinkMessageReceiver.h>

#define DEBUG 1

ArdulinkMessageReceiver Receiver;

boolean handleCustomMessage(String message);

void keyPressStub(char key) {}

HCSR04Ultrasonic frontSensor(52, 53, 50, 51);
HCSR04Ultrasonic leftSensor(28, 29, 26, 27);

// left motor
const int analogPinLeft = 9; //analogPinLeft
const int digitalPinLeft = 22; //digitalPinLeft

// right motor
const int analogPinRight = 10; //analogPinRight
const int digitalPinRight = 23; //digitalPinRight

long durationFront, durationLeft;
double inchesFront, inchesLeft;

// timer for ultrasonic
unsigned long const interval = 70;
unsigned long previousMillis = 0;

int PWMValRight = 127;
int PWMValLeft = 127;

// boolean to set motors to off, to avoid gradual roll
bool motorOff, frontStop;

const int lowThreshold = 7;
const int threshold = 11;
const int highThreshold = 100;

enum events {none, rangeBelow, rangeAbove, rangeHigh};

events event = none;

void setup() {
  // initialize Ardulink
  if (!DEBUG)
    Receiver.init(handleCustomMessage, keyPressStub);
  else
    Serial.begin(9600);

  // initialize sensors
  frontSensor.init();
  leftSensor.init();
  //rightSensor.init();

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
  analogWrite(analogPinLeft, PWMValLeft);
  analogWrite(analogPinRight, PWMValRight);

  // set motors to off
  motorOff = true;
  frontStop = true;
}

void loop() {
  if (!DEBUG)
    Receiver.processInput();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    inchesFront = frontSensor.getDistanceIn();

    inchesLeft = leftSensor.getDistanceIn();

    //Serial.println("Left");
    Serial.print(inchesLeft);
    Serial.print(" inLeft");
    Serial.println();
  }
  ObstacleInFront();
  if (!frontStop) { WallFollow(); }
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

///////////////Motor Functions/////////////////
void boundsCheck() {
  if (PWMValRight > 248) PWMValRight = 248;
  
  if (PWMValLeft > 248) PWMValLeft = 248;
  
  if (PWMValRight < 8) PWMValRight = 8;
  
  if (PWMValLeft < 8) PWMValLeft = 8;
}

void userDisable() {
  PWMValLeft = 127;
  PWMValRight = 127;
  digitalWrite(digitalPinRight, HIGH);
  digitalWrite(digitalPinLeft, HIGH);
}

void userEnable() {
  digitalWrite(digitalPinRight, LOW);
  digitalWrite(digitalPinLeft, LOW);

  PWMValLeft = 127;
  PWMValRight = 127;
}

void setMotorSpeed() {
  digitalWrite(digitalPinRight, LOW);
  analogWrite(analogPinRight, PWMValRight);
  digitalWrite(digitalPinLeft, LOW);
  analogWrite(analogPinLeft, PWMValLeft);
}

void updateSpeed() {
  boundsCheck();
  setMotorSpeed();
}
////////////////////////////////

////////////Functions for Ultrasonic Events////////////////
void ObstacleInFront() {
  if(inchesFront <= 24 && inchesFront >= 0) {
    userDisable();
    frontStop = true;
    /*
    PWMValLeft = 190;
    PWMValRight = 66;

    updateSpeed();

    delay(2000);

    userDisable();
    updateSpeed();
    */
  }
  else {
    userEnable();
    frontStop = false;
    /*
    if (!motorOff) {
      userEnable();
      PWMValRight = 220;
      PWMValLeft = 220;
      updateSpeed();
    }
    else {
      userDisable();
    }
    */
  }
}

void WallFollow() {
  //if no wall on right side
  if (inchesLeft > highThreshold) {
    event = rangeHigh;
  }
  else if (inchesLeft > threshold && inchesLeft < highThreshold) {
    event = rangeAbove;
  }
  else if (inchesLeft < lowThreshold) {
    event = rangeBelow;
  }
  else {
    event = none;
  }

  switch(event) {
    case rangeBelow:  CloseToWall();
                      break;
    case rangeAbove:  FarFromWall();
                      break;
    case rangeHigh:   NoWallDetected();
                      break;
    default:          userEnable();
                      PWMValLeft = 220;
                      PWMValRight = 220;
                      updateSpeed();
                      break;
  }  
}

void CloseToWall() {
  //stop
  ClearMotors();
  //turn left
  TurnLeft();
  delay(500);
  ClearMotors();
  SlowForward();
  delay(600);
  ClearMotors();
  TurnRight();
  delay(600);
  userDisable();
  delay(50);
}

void FarFromWall() {
  //stop
  ClearMotors();
  //turn right
  TurnRight();
  delay(500);
  ClearMotors();
  SlowForward();
  delay(600);
  ClearMotors();
  TurnLeft();
  delay(600);
  userDisable();
  delay(50);
}

void NoWallDetected() {
  // go forward for some time
  // turn right
  ClearMotors();
  PWMValLeft = 200;
  PWMValRight = 200;
  updateSpeed();
  delay(300);
  PWMValLeft = 74;
  PWMValRight = 180;
  updateSpeed();
  delay(2500);
  PWMValLeft = 200;
  PWMValRight = 200;
  updateSpeed();
  delay(500);
}

void ClearMotors() {
  userDisable();
  delay(100);
  userEnable();
}

void TurnRight() {
  PWMValLeft = 94;
  PWMValRight = 160;
  updateSpeed();
}

void TurnLeft() {
  PWMValLeft = 160;
  PWMValRight = 94;
  updateSpeed();
}

void SlowForward() {
  PWMValLeft = 180;
  PWMValRight = 180;
  updateSpeed();
}
///////////////////////////////////////////


