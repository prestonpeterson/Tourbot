/*
This class allows control of the HC-SR04 Ultrasonic sensor
*/
#include <Arduino.h>
#include "HCSR04Ultrasonic.h"

// constructor for when using dedicated voltage and ground pins
HCSR04Ultrasonic::HCSR04Ultrasonic(int triggerPinNum, int echoPinNum) {
	triggerPin = triggerPinNum;
	echoPin = echoPinNum;
	voltagePin = -1;
	groundPin = -1;
}

// constructor for when not using dedicated voltage and ground pins
HCSR04Ultrasonic::HCSR04Ultrasonic(int triggerPinNum, int echoPinNum, int voltPinNum, int groundPinNum){
	triggerPin = triggerPinNum;
	echoPin = echoPinNum;
	voltagePin = voltPinNum;
	groundPin = groundPinNum;
}

unsigned long HCSR04Ultrasonic::getDistanceRaw(){
	// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	digitalWrite(triggerPin, LOW);
	delayMicroseconds(5);
	digitalWrite(triggerPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(triggerPin, LOW);

	// Read the signal from the sensor: a HIGH pulse whose
	// duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	return pulseIn(echoPin, HIGH);
}

double HCSR04Ultrasonic::getDistanceCm(){
	return (getDistanceRaw() / 2.0) / 29.1;
	
}

double HCSR04Ultrasonic::getDistanceIn(){
	return (getDistanceRaw() / 2.0) / 74;
}

void HCSR04Ultrasonic::init(){
	pinMode(triggerPin, OUTPUT);
	pinMode(echoPin, INPUT);
	if (voltagePin > 0) {
		pinMode(voltagePin, OUTPUT);
		digitalWrite(voltagePin, HIGH);
	}	
	if (groundPin > 0) {
		pinMode(groundPin, OUTPUT);
		digitalWrite(groundPin, LOW);
	}
}