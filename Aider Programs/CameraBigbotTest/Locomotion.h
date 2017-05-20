/* 
 * Header file that contains functions that drive the motion 
 * of the robot.
 */
#ifndef LOCOMOTION
#define LOCOMOTION
// left motor
const int analogPinLeft = 9; //analogPinLeft
const int digitalPinLeft = 22; //digitalPinLeft

// right motor
const int analogPinRight = 10; //analogPinRight
const int digitalPinRight = 23; //digitalPinRight

int PWMValRight; // = 127;
int PWMValLeft; // = 127;

void ClearMotors();

void InitializeMotors() {
  // motor control
  pinMode(digitalPinLeft, OUTPUT);
  pinMode(digitalPinRight, OUTPUT);
  
  // initial motor values
  ClearMotors();
}

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

void ClearMotors() {
  userDisable();
  delay(100);
  userEnable();
}

void IdleMotors() {
  PWMValLeft = 127;
  PWMValRight = 127;
  updateSpeed();
}

void TurnLeft() {
  PWMValLeft = 94;
  PWMValRight = 155;
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
#endif
