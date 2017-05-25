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

// function to initialize the digital control of the motors
// and to set the motors to ~50% pwm, which should keep them from moving
// for the most part
void InitializeMotors() {
  // motor control
  pinMode(digitalPinLeft, OUTPUT);
  pinMode(digitalPinRight, OUTPUT);
  
  // initial motor values
  ClearMotors();
}

// a check to ensure that the pwm values sent are not out of bounds
// we used 8-248 to allow us the use increments of 10
void boundsCheck() {
  if (PWMValRight > 248) PWMValRight = 248;
  
  if (PWMValLeft > 248) PWMValLeft = 248;
  
  if (PWMValRight < 8) PWMValRight = 8;
  
  if (PWMValLeft < 8) PWMValLeft = 8;
}

// disables the motors by sending a high digital signal
// also sets the pwm values to 127 in order to overwrite their previous values
// this prevents the robot from suddenly moving when it is re-enabled
void userDisable() {
  PWMValLeft = 127;
  PWMValRight = 127;
  digitalWrite(digitalPinRight, HIGH);
  digitalWrite(digitalPinLeft, HIGH);
}

// enables use of the motors
// pwm values are set to 127 to make sure the robot does not immediately start moving
// due to a previous value
void userEnable() {
  digitalWrite(digitalPinRight, LOW);
  digitalWrite(digitalPinLeft, LOW);

  PWMValLeft = 127;
  PWMValRight = 127;
}

// sends the pwm signal to the motors
// and a digital low signal to make sure motors are enabled
void setMotorSpeed() {
  digitalWrite(digitalPinRight, LOW);
  analogWrite(analogPinRight, PWMValRight);
  digitalWrite(digitalPinLeft, LOW);
  analogWrite(analogPinLeft, PWMValLeft);
}

// calls bounds check to prevent out of bounds errors
// then sends the pwm values to the motors
void updateSpeed() {
  boundsCheck();
  setMotorSpeed();
}

// disables the motors and resets the pwm values for left and right to ~50%
// then enables the motors
void ClearMotors() {
  userDisable();
  delay(100);
  userEnable();
}

// sets the pwm values for both sides to ~50%
// then updates speed
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
