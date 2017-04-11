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
	/**
	* Initializes the serial communications and variables needed, call in setup()
	* @param customMessageHandler A function called whenever a custom message is recieved, given the message as a string
	* @param keyPressMessageHandler A function called whenever a keypress message is recieved, given the character pressed
	*/
	void init(bool (*customMessageHandler)(String), void (*keyPressMessageHandler)(char));

	/**
	* Handles a new Ardulink message (if any) and then sends pin listener messages (if any are assigned), call in loop()
	*/
	void processInput();

	/**
	* Reads new characters sent over serial communication and stores them for processInput(), call in serialEvent()
	*/
	void getInput();
protected:
	String inputString = "";        // a string to hold incoming data
	bool stringComplete = false;	// whether the string is complete

	bool digitalPinListening[numDigitalPins]; // Array used to know which pins on the Arduino must be listening.
	bool analogPinListening[numAnalogPins]; // Array used to know which pins on the Arduino must be listening.
	int digitalPinListenedValue[numDigitalPins]; // Array used to know which value is read last time.
	int analogPinListenedValue[numAnalogPins]; // Array used to know which value is read last time.

	bool (*handleCustomMessage)(String);
	void (*handleKeyPressMessage)(char);

	void sendReplyMessage(bool validMessage);
	void sendListenMessages();
	int highPrecisionAnalogRead(int pin);
};

#endif