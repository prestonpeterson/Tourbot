/*
 * This class allows control of the HC-SR04 Ultrasonic sensor
 *
 * The constructor with two parameters should be used if the ultrasonic's voltage and ground pins are
 * plugged into dedicated VCC and GND pins on the Arduino. The constructor with four parameters should
 * be used if the voltage and trigger pins are plugged into numbered digital pins.
 *
 * The function getDistanceRaw() can be used to calculate distance to an object in both inches and
 * centimeters, but getDistanceCM() and getDistanceIn() are the main functions that should be used.
 *
 * NOTE: There must be a minimum of 60 ms between distance measurements to ensure echoes from previous
 * signals do not interfere with the current measurement.
 */

#ifndef HCSR04ULTRASONIC
#define HCSR04ULTRASONIC

class HCSR04Ultrasonic{
public:
	HCSR04Ultrasonic(int triggerPinNum, int echoPinNum);
	HCSR04Ultrasonic(int triggerPinNum, int echoPinNum, int voltPinNum, int groundPinNum);
	void init();
	unsigned long getDistanceRaw(); //Returns miliseconds between a soundwave being sent and the echo returning
	double getDistanceCm(); //Returns distance from the sensor to an object in centimeters
	double getDistanceIn(); //Returns distance from the sensor to an object in inches
private:
	int voltagePin, groundPin, triggerPin, echoPin;
};

#endif