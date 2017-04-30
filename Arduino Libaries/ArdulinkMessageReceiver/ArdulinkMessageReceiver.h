/*
 * This class implements a modified version of the Ardulink Protocol for serial communication.
 *
 * Most of the code for this class is taken from an example program that sent and recieved Ardulink
 * messages. Some improvements/changes have been made but most of the code is not originally ours.
 * There is a lot of functionality this code implements that were not at all relevant to us but are
 * left included as they may be relevant in the future. These features include being able to listen for
 * changes in pin values and report them over serial and the ability to let a java program control
 * pin output values with Ardulink.
 *
 * The focus of our implementation is the custom messages (which simply allows a string to be sent
 * from a java program to the Arduino program). When a custom message is received, a function is 
 * called and given the string as a parameter; the function called is given as a function pointer
 * in the constructor and can respond as necessary to the incoming string. The function must also
 * return a boolean reporting whether the string was recognized, which is used to inform the
 * java program (if necessary).
 *
 * There is also a function pointer implemented for keyPress messages that operates in the same way
 * as the custom messages, but it does not return a boolean for recognizing the message. The keyPress
 * message cannot be sent by the ArduinoCore library (only the Arduino Swing libray) but the ability
 * to receive the message is again included for completeness and future-proofing.
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
	* Handles a new Ardulink message (if any) and then sends pin listener messages (if any are assigned),
	* call at the start of loop()
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

	bool (*handleCustomMessage)(String); // Called whenever a customMessage is recieved and given the string sent, returns if the string was recognized.
	void (*handleKeyPressMessage)(char); // Called when a keyPressMessage is recieved and given the character sent

	void sendReplyMessage(bool validMessage); // If the Ardulink message indicated a reply was required, this sends a reply stating if the message was recognized
	void sendListenMessages(); // Sends values of pins that have been set to listen to them; only sends values if they have changed
	int highPrecisionAnalogRead(int pin); // A weird function used in the example program that takes 4 analogReads and averages them.
};

#endif