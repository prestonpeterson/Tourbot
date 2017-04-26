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

enum turns {nothing, left, right};
turns lastTurnExecuted = nothing;

// timer for ultrasonic
unsigned long const interval = 70;
unsigned long previousMillis = 0;

int PWMValRight = 127;
int PWMValLeft = 127;

// boolean to set motors to off, to avoid gradual roll
bool motorOff, frontStop;
bool checkingForObstacle = false;

const double lowThreshold = 8;
const double threshold = 12;
const int highThreshold = 100;

enum events {none, rangeBelow, rangeAbove, rangeHigh};

events event = none;

void setup() {
  // initialize Ardulink
  if (!DEBUG)
    Receiver.init(handleCustomMessage, keyPressStub);
  else
    Serial.begin(9600);

  Serial.println("Initializing");
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
  ClearMotors();
  
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

    inchesLeft = leftSensor.getDistanceIn();
  }
  if (!ObstacleInFront()) WallFollow(); 
  else StopWhileObstacleInFront();
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
boolean ObstacleInFront() {
  inchesFront = frontSensor.getDistanceIn();
  if(inchesFront <= 24 && inchesFront >= 0) {
    delay(70);
    inchesFront = frontSensor.getDistanceIn();
    if(inchesFront <= 24 && inchesFront >= 0) {
      userDisable();
      frontStop = true;
    }
    /*
    PWMValLeft = 190;
    PWMValRight = 66;
    updateSpeed();
    delay(2000);
    userDisable();
    updateSpeed();
    */
    return true;
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
    return false;
  }
}

void StopWhileObstacleInFront() {
  Serial.print("OBSTACLE IN PATH! Inches: ");
  PrintInches(inchesFront);
  while (frontStop) {
      delay(2000);
      double dist1 = frontSensor.getDistanceIn();
      Serial.print("Front inches1: ");
      PrintInches(dist1);
      delay(70);
      double dist2 = frontSensor.getDistanceIn();
      Serial.print("Front inches2: ");
      PrintInches(dist2);
      delay(70);
      double dist3 = frontSensor.getDistanceIn();
      Serial.print("Front inches3: ");
      PrintInches(dist3);
      if (dist1 > 24 && dist2 > 24 && dist3 > 24) {
        userEnable();
        frontStop = false;
        delay(70);
      }
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
    default:          Serial.println("Cruising along wall...");
                      userEnable();
                      PWMValLeft = 220;
                      PWMValRight = 220;
                      updateSpeed();
                      break;
  }  
}

void CloseToWall() {
  Serial.print("Too close to wall! Inches: ");
  PrintInches(inchesLeft);
  ClearMotors(); // 100 ms
 
  // check distance
  double inchesLeft = leftSensor.getDistanceIn();
  TurnRight();
  delay(700);
  if (ObstacleInFront()) StopWhileObstacleInFront();
  if (lastTurnExecuted == left) {
    Serial.println("Going forward slowly for 500 ms");
    SuperSlowForward();
    delay(500);
    if (ObstacleInFront()) StopWhileObstacleInFront();
  }
  
  lastTurnExecuted = right;
  ClearMotors();
  double newInchesLeft = leftSensor.getDistanceIn();
  if (newInchesLeft > inchesLeft) {
    Serial.println("Farther from wall after turning; now moving forward...");
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newInchesLeft < lowThreshold + 2) {
      SlowForward();
      delay(70);
      if (ObstacleInFront()) StopWhileObstacleInFront();

      newInchesLeft = leftSensor.getDistanceIn();
    }
    TurnLeft();
    delay(600);
    if (ObstacleInFront()) StopWhileObstacleInFront();
  }
  else {
    Serial.println("Not further from wall after turning; must keep turning...");
    ClearMotors();
    TurnRight();
    delay(500);
    if (ObstacleInFront()) StopWhileObstacleInFront();
    SlowForward();
    delay(300);
    if (ObstacleInFront()) StopWhileObstacleInFront();
    ClearMotors();
    userDisable();
    delay(50);
  }
  
}

void FarFromWall() {
  Serial.print("Too far from wall! Inches: ");
  PrintInches(inchesLeft);
  //stop
  ClearMotors();
  //turn left
  // check distance
  double inchesLeft = leftSensor.getDistanceIn();
  Serial.print("Inches left: ");
  PrintInches(inchesLeft);
  TurnLeft();
  delay(700);
  if (ObstacleInFront()) StopWhileObstacleInFront();
  if (lastTurnExecuted == right) {
    Serial.println("Going forward slowly for 500 ms");
    SuperSlowForward();
    delay(500);
    if (ObstacleInFront()) StopWhileObstacleInFront();
  }
  lastTurnExecuted = left;
  ClearMotors();
  double newInchesLeft = leftSensor.getDistanceIn();
  Serial.print("Inches left: ");
  PrintInches(newInchesLeft);
  if (newInchesLeft < inchesLeft) {
    Serial.println("Closer to wall after turning; now moving forward...");
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newInchesLeft > threshold + 3) {
      SlowForward();
      delay(70);
      if (ObstacleInFront()) StopWhileObstacleInFront();
      newInchesLeft = leftSensor.getDistanceIn();
    }
    TurnRight();
    delay(600);
    if (ObstacleInFront()) StopWhileObstacleInFront();
  }
  else {
    Serial.println("Not closer to wall after turning; must keep turning...");
    ClearMotors();
    TurnLeft();
    delay(500);
    if (ObstacleInFront()) StopWhileObstacleInFront();
    SlowForward();
    delay(400);
    if (ObstacleInFront()) StopWhileObstacleInFront();
    ClearMotors();
    userDisable();
    delay(50);
  }
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

void TurnLeft() {
  PWMValLeft = 94;
  PWMValRight = 135;
  updateSpeed();
}

void TurnRight() {
  PWMValLeft = 190;
  PWMValRight = 94;
  updateSpeed();
}

void SlowForward() {
  PWMValLeft = 180;
  PWMValRight = 180;
  updateSpeed();
}

void SuperSlowForward() {
  PWMValLeft = 165;
  PWMValRight = 165;
  updateSpeed();
}

void PrintInches(int inches) {
    Serial.print(inches);
    Serial.print(" inLeft");
    Serial.println();
}
///////////////////////////////////////////


