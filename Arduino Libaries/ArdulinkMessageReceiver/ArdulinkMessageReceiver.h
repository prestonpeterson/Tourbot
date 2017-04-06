/*
This class implements a modified version of the Ardulink Protocol for serial 
communication.
*/

#ifndef ARDULINKMESSAGERECEIVER
#define ARDULINKMESSAGERECEIVER

//*******************************************************************************************
//These values are Arduino Mega specific and must be modified for use on other Arduino boards
#define numDigitalPins 70
#define numAnalogPins 16
#define firstAnalogPinNum 54
//*******************************************************************************************

class ArdulinkMessageReceiver{
public:
	void init(); // Call in setup()
	void processInput(); // Call in loop()
	void getInput(); // Call in serialEvent()
protected:
	String inputString = "";        // a string to hold incoming data
	bool stringComplete = false;	// whether the string is complete

	bool digitalPinListening[numDigitalPins]; // Array used to know which pins on the Arduino must be listening.
	bool analogPinListening[numAnalogPins]; // Array used to know which pins on the Arduino must be listening.
	int digitalPinListenedValue[numDigitalPins]; // Array used to know which value is read last time.
	int analogPinListenedValue[numAnalogPins]; // Array used to know which value is read last time.

	void sendReplyMessage(bool validMessage);
	void sendListenMessages();
	int highPrecisionAnalogRead(int pin);
};

#endif