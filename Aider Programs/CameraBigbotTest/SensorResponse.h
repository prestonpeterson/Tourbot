/*
 * Functions that 
 */
#ifndef SENSORRESPONSE
#define SENSORRESPONSE
const int lowThreshold = 8;
const int highThreshold = 12;
const int maxThreshold = 100;

enum events {none, rangeBelow, rangeAbove, rangeHigh};
enum turns {nothing, towardWall, awayFromWall};
enum walls {leftWall, rightWall};

events event = none;
turns lastTurnExecuted = nothing;
walls currentWall = rightWall;

bool checkingForObstacle = false;
bool frontStop;
bool isCruising = false;

// trigger, echo, volt, ground
//HCSR04Ultrasonic frontRSensor();
HCSR04Ultrasonic rightSensor(48, 49, 46, 47);
HCSR04Ultrasonic frontSensor(52, 53, 50, 51);
HCSR04Ultrasonic leftSensor(28, 29, 26, 27);


void PrintInches(double inches) {
    DebugPrint(inches);
    DebugPrintln();
}

double GetInchesFromWall() {
  // TODO: add millis check to this function, and take it out of the rest of the code
  if (currentWall == leftWall) return leftSensor.getDistanceIn();
  else return rightSensor.getDistanceIn();
}

void TurnTowardsWall() {
  if (currentWall == leftWall) TurnLeft();
  else TurnRight();
}

void TurnAwayFromWall() {
  if (currentWall == leftWall) TurnRight();
  else TurnLeft();
}

void StopUntilPathClear(double inchesFront) {
  return;
  DebugPrint("OBSTACLE IN PATH! Inches: ");
  PrintInches(inchesFront);
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
      inchesFront = frontSensor.getDistanceIn();
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

void StopIfPathObstructed() {
  double inchesFront = frontSensor.getDistanceIn();
  if(inchesFront <= 24 && inchesFront >= 0) {
    delay(70);
    inchesFront = frontSensor.getDistanceIn();
    if(inchesFront <= 24 && inchesFront >= 0) {
      userDisable();
      frontStop = true;
      StopUntilPathClear(inchesFront);
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
    SuperSlowForward();
    delay(500);
    StopIfPathObstructed();
  }
  
  lastTurnExecuted = awayFromWall;
  ClearMotors();
  double newdist = GetInchesFromWall();
  if (newdist > dist + 0.4) {
    DebugPrintln("Farther from wall after turning; now moving forward...");
    int initialInches = newdist;
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newdist < lowThreshold + 2) {
      SlowForward();
      delay(70);
      StopIfPathObstructed();

      newdist = GetInchesFromWall();
      if (newdist < initialInches - 0.5) {
        TurnAwayFromWall();
        delay(350);
        return;
      }
    }
    TurnTowardsWall();
    delay(600);
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
  StopIfPathObstructed();
  if (lastTurnExecuted == awayFromWall) {
    DebugPrintln("Going forward slowly for 500 ms");
    SuperSlowForward();
    delay(500);
    StopIfPathObstructed();
  }
  lastTurnExecuted = towardWall;
  ClearMotors();
  double newdist = GetInchesFromWall();
  DebugPrint("Inches from wall: ");
  PrintInches(newdist);
  if (newdist < dist - 0.4) {
    int initialInches = newdist;
    DebugPrintln("Closer to wall after turning; now moving forward...");
    // while distance > maxRange: go forward, delay 250ms, check distance again
    while (newdist > highThreshold + 3) {
      SlowForward();
      delay(70);
      StopIfPathObstructed();
      newdist = GetInchesFromWall();
      if (newdist > initialInches + 0.5) {
        TurnTowardsWall();
        delay(350);
        return;
      }
    }
    dist = newdist;
    TurnAwayFromWall();
    delay(600);
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
