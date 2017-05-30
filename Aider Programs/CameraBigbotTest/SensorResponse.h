/*
 * Functions that make use of the data collected from the sensors.
 * Currently only ultrasonic sensors.
 */
#ifndef SENSORRESPONSE
#define SENSORRESPONSE
// the following values are in inches
const int lowThreshold = 8; // threshold for being too close to wall
const int highThreshold = 12; // threshold for being too far from wall
const int maxThreshold = 100; // threshold to know when a wall is not detected

/* Ultrasonic sensor events:
 * rangeBelow: too close to wall
 * rangeAbove: too far from wall
 * rangeHigh: no wall detected, possibly in an intersection of hallways
 */
enum events {none, rangeBelow, rangeAbove, rangeHigh};
enum turns {nothing, towardWall, awayFromWall}; // to keep track of last turn that was executed
enum walls {leftWall, rightWall}; // allow switching between right wall-following or left wall-following
// currently, to switch which wall to follow, the current ultrasonic must be unplugged and replaced with the
// side for the desired wall-following

events event = none;
turns lastTurnExecuted = nothing;
walls currentWall = leftWall;

bool checkingForObstacle = false;
bool frontStop;
bool isCruising = false;

// HCSR04Ultrasonic variableName( triggerPin, echoPin, volt, ground )
HCSR04Ultrasonic sideSensor(52, 53, 50, 51);
HCSR04Ultrasonic frontSensor(28, 29, 26, 27);

// used for debugging the ultrasonic sensor values
void PrintInches(double inches) {
    DebugPrint(inches);
    DebugPrintln();
}

double GetInchesFromWall() {
  // TODO: add millis check to this function, and take it out of the rest of the code
  return sideSensor.getDistanceIn();
}

void TurnTowardsWall() {
  if (currentWall == leftWall) TurnLeft();
  else TurnRight();
}

void TurnAwayFromWall() {
  if (currentWall == leftWall) TurnRight();
  else TurnLeft();
}

void StopUntilPathClear() {
  while (frontStop) {
      delay(2000);
      double dist1 = frontSensor.getDistanceIn();
      DebugPrint("Front inches1: ");
      PrintInches(dist1);
      delay(70);
      double dist2 = frontSensor.getDistanceIn();
      DebugPrint("Front inches2: ");
      PrintInches(dist2);
      delay(70);
      double inchesFront = frontSensor.getDistanceIn();
      DebugPrint("Front inches3: ");
      PrintInches(inchesFront);
      if (dist1 > 24 && dist2 > 24 && inchesFront > 24) {
        userEnable();
        frontStop = false;
        DebugPrint("PATH CLEAR!");
        delay(70);
      }
  }
}

void DebugSensors() {
  delay(70);
  double side = sideSensor.getDistanceIn();
  double front = frontSensor.getDistanceIn();
  DebugPrint("Side: ");
  DebugPrint(side);
  DebugPrint(". Front: ");
  DebugPrintln(front);
}

void StopIfPathObstructed() {
  double inchesFront = frontSensor.getDistanceIn();
  if(inchesFront <= 24 && inchesFront >= 0) {
    delay(70);
    inchesFront = frontSensor.getDistanceIn();
    if(inchesFront <= 24 && inchesFront >= 0) {
      DebugPrintln("PATH OBSTRUCTED!");
      DebugPrint("Inches from obstacle: ");
      PrintInches(inchesFront);
      userDisable();
      frontStop = true;
      StopUntilPathClear();
    }
  }
  else {
    userEnable();
    frontStop = false;
  }
}

void CloseToWall() {
  // check distance
  double dist = GetInchesFromWall();
  DebugPrint("CloseToWall()! Inches: ");
  PrintInches(dist);
  ClearMotors(); 
  TurnAwayFromWall();
  delay(700);
  StopIfPathObstructed();
  if (lastTurnExecuted == towardWall) {
    DebugPrintln("Going forward slowly for 500 ms");
    SlowForward();
    delay(500);
    StopIfPathObstructed();
  }
  
  lastTurnExecuted = awayFromWall;
  //ClearMotors();
  IdleMotors();
  double newdist = GetInchesFromWall();
  if (newdist > dist + 0.4) {
    DebugPrintln("Farther from wall after turning; now moving forward...");
    int initialInches = newdist;
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newdist < lowThreshold + 2) {
      Receiver.getInput(); // Ardulink messages processed here because of the while loop
      Receiver.processInput();
      if (motorOff)
        return;
      
      SlowForward();
      delay(70);
      StopIfPathObstructed();

      newdist = GetInchesFromWall();
      if (newdist < initialInches - 0.5) {
        TurnAwayFromWall();
        delay(450);
        return;
      }
    }
    TurnTowardsWall();
    delay(300);
    StopIfPathObstructed();
  }
}

void FarFromWall() {
  
  //stop
  ClearMotors();
  //turn left
  // check distance
  double dist = GetInchesFromWall();
  DebugPrintln("***FarFromWall()***");
  DebugPrint("Inches from wall: ");
  PrintInches(dist);
  TurnTowardsWall();
  delay(700);
  dist = GetInchesFromWall();
  StopIfPathObstructed();
  //if (lastTurnExecuted == awayFromWall) {
    DebugPrintln("Going forward slowly for 500 ms");
    SlowForward();
    delay(500);
    StopIfPathObstructed();
  //}
  lastTurnExecuted = towardWall;
  IdleMotors();
  double newdist = GetInchesFromWall();
  DebugPrint("Inches from wall: ");
  PrintInches(newdist);
  if (newdist < dist - 0.3) {
    int initialInches = newdist;
    DebugPrintln("Closer to wall after turning; now moving forward...");
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newdist > highThreshold + 3) {
      Receiver.getInput(); // Ardulink messages processed here because of the while loop
      Receiver.processInput();
      if (motorOff)
        return;
      
      SlowForward();
      delay(300);
      StopIfPathObstructed();
      newdist = GetInchesFromWall();
      if (newdist > initialInches + 0.5) {
        TurnTowardsWall();
        delay(350);
        return;
      }
    }
    dist = newdist;
    //TurnAwayFromWall();
    delay(300);
    StopIfPathObstructed();
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

void WallFollow(double dist) {
  //if no wall on right side
  if (dist > maxThreshold) {
    event = rangeHigh;
  }
  else if (dist > highThreshold && dist < maxThreshold) {
    event = rangeAbove;
  }
  else if (dist < lowThreshold) {
    event = rangeBelow;
  }
  else {
    event = none;
  }

  switch(event) {
    case rangeBelow:  isCruising = false;
                      CloseToWall();
                      break;
    case rangeAbove:  isCruising = false;
                      FarFromWall();
                      break;
    case rangeHigh:   isCruising = false;
                      NoWallDetected();
                      break;
    default:          if (!isCruising) DebugPrintln("Cruising along wall...");
                      isCruising = true;
                      userEnable();
                      PWMValLeft = 220;
                      PWMValRight = 220;
                      updateSpeed();
                      break;
  }  
}
#endif
