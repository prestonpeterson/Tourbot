/*
This class allows control of the HC-SR04 Ultrasonic sensor
*/

#ifndef HCSR04ULTRASONIC
#define HCSR04ULTRASONIC

class HCSR04Utrasonic{
public:
	HCSR04Ultrasonic(int triggerPinNum, int echoPinNum);
	HCSR04Ultrasonic(int triggerPinNum, int echoPinNum, int voltPinNum, int groundPinNum);
	int getDistanceRaw();
	double getDistanceCm();
	double getDistanceIn();
private:
	int voltagePin, groundPin, triggerPin, echoPin;

	void init();
};

#endif