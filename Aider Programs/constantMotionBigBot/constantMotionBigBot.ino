/*
 * Program to control big bot.
 * Key presses control speed of motors, and speed stays constant
 */

#include <Wire.h>

// left motor
int analogPinLeft = 9; //analogPinLeft
int digitalPinLeft = 22; //digitalPinLeft

// right motor
int analogPinRight = 10; //analogPinRight
int digitalPinRight = 23; //digitalPinRight

void setup() {
  Serial.begin(9600);

  //set pins to ground
  pinMode(24, OUTPUT);
  pinMode(25, OUTPUT);
  digitalWrite(24, LOW);
  digitalWrite(25, LOW);

  pinMode(digitalPinLeft, OUTPUT);

  pinMode(digitalPinRight, OUTPUT);
  // changes timer
  int prescalerval = 0x07;
  TCCR2B &= ~prescalerval;
  prescalerval = 2;
  TCCR2B |= prescalerval;

  digitalWrite(digitalPinLeft, LOW);
  digitalWrite(digitalPinRight, LOW);
  analogWrite(analogPinLeft, 128);
  analogWrite(analogPinRight, 128);
}

int PWMValRight = 128;
int PWMValLeft = 128;

void loop() {
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
      case 'm': PWMValLeft = 198;
                PWMValRight = 198;
                updateSpeed();
                break;
      case 'n': PWMValLeft = 58;
                PWMValRight = 58;
                updateSpeed();
                break;
      case 'p': PWMValLeft = 58;
                PWMValRight = 198;
                updateSpeed();
                break;
      case 'o': PWMValLeft = 198;
                PWMValRight = 58;
                updateSpeed();
                break;
      case '0': userDisable();
                break;
      case '1': userEnable();
                break;
      default:  //all stop
                PWMValLeft = 127;
                PWMValRight = 127;
                updateSpeed();
                break;
      }
  }
  delay(250);
}

void updateSpeed() {
  boundsCheck();
  setMotorSpeed();
  printSpeeds();
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
  Serial.println("\nDisable Motors\n");
}

void userEnable() {
  digitalWrite(digitalPinRight, LOW);
  digitalWrite(digitalPinLeft, LOW);
  Serial.println("\nEnable Motors\n");
}

