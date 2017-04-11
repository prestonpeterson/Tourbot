/*
This class implements a modified version of the Ardulink Protocol for serial
communication.
*/

#include <Arduino.h>
#include "ArdulinkMessageReceiver.h"

void ArdulinkMessageReceiver::init(bool(*customMessageHandler)(String)) {
	inputString = "";
	stringComplete = false;
	handleCustomMessage = customMessageHandler;

	Serial.begin(115200);

	Serial.print("alp://rply/");
	Serial.print("ok?id=0");
	Serial.print('\n'); // End of Message
	Serial.flush();

	//set to false all listen variable
	int index = 0;
	for (index = 0; index < numDigitalPins; index++) {
		digitalPinListening[index] = false;
		digitalPinListenedValue[index] = -1;
	}
	for (index = firstAnalogPinNum; index < numAnalogPins; index++) {
		analogPinListening[index] = false;
		analogPinListenedValue[index] = -1;
	}

	/* But why?

	// Turn off everything (not on RXTX)
	for (index = 2; index < digitalPinListeningNum; index++) {
		pinMode(index, OUTPUT);
		digitalWrite(index, LOW);
	}

	*/
}

void ArdulinkMessageReceiver::processInput() {
	if (stringComplete) {

		if (inputString.startsWith("alp://")) { // OK is a message I know

			boolean msgRecognized = true;

			if (inputString.substring(6, 10) == "cust") { // Custom Message
				int messageIdPosition = inputString.indexOf('?', 11);
				String customMessage;
				if (messageIdPosition > -1)
					customMessage = inputString.substring(11, messageIdPosition);
				else
					customMessage = inputString.substring(11, inputString.length() - 1); //Ensures \n at end of string is not copied

				msgRecognized = handleCustomMessage(customMessage);
			}
			else if (inputString.substring(6, 10) == "kprs") { // KeyPressed
				// here you can write your own code. For instance the commented code change pin intensity if you press 'a' or 's'
				// take the command and change intensity on pin 11 this is needed just as example for this sketch
				//char commandChar = inputString.charAt(14);
				//if(commandChar == 'a' and intensity > 0) { // If press 'a' less intensity
				//  intensity--;
				//  analogWrite(11,intensity);
				//} else if(commandChar == 's' and intensity < 125) { // If press 's' more intensity
				//  intensity++;
				//  analogWrite(11,intensity);
				//}
			}
			else if (inputString.substring(6, 10) == "ppin") { // Power Pin Intensity
				int separatorPosition = inputString.indexOf('/', 11);
				String pin = inputString.substring(11, separatorPosition);
				String intens = inputString.substring(separatorPosition + 1);
				pinMode(pin.toInt(), OUTPUT);
				analogWrite(pin.toInt(), intens.toInt());
			}
			else if (inputString.substring(6, 10) == "ppsw") { // Power Pin Switch
				int separatorPosition = inputString.indexOf('/', 11);
				String pin = inputString.substring(11, separatorPosition);
				String power = inputString.substring(separatorPosition + 1);
				pinMode(pin.toInt(), OUTPUT);
				if (power.toInt() == 1) {
					digitalWrite(pin.toInt(), HIGH);
				}
				else if (power.toInt() == 0) {
					digitalWrite(pin.toInt(), LOW);
				}
			}
			else if (inputString.substring(6, 10) == "tone") { // tone request
				int firstSlashPosition = inputString.indexOf('/', 11);
				int secondSlashPosition = inputString.indexOf('/', firstSlashPosition + 1);
				int pin = inputString.substring(11, firstSlashPosition).toInt();
				int frequency = inputString.substring(firstSlashPosition + 1, secondSlashPosition).toInt();
				int duration = inputString.substring(secondSlashPosition + 1).toInt();
				if (duration == -1) {
					tone(pin, frequency);
				}
				else {
					tone(pin, frequency, duration);
				}
			}
			else if (inputString.substring(6, 10) == "notn") { // no tone request
				int firstSlashPosition = inputString.indexOf('/', 11);
				int pin = inputString.substring(11, firstSlashPosition).toInt();
				noTone(pin);
			}
			else if (inputString.substring(6, 10) == "srld") { // Start Listen Digital Pin
				String pin = inputString.substring(11);
				digitalPinListening[pin.toInt()] = true;
				digitalPinListenedValue[pin.toInt()] = -1; // Ensure a message back when start listen happens.
				pinMode(pin.toInt(), INPUT);
			}
			else if (inputString.substring(6, 10) == "spld") { // Stop Listen Digital Pin
				String pin = inputString.substring(11);
				digitalPinListening[pin.toInt()] = false;
				digitalPinListenedValue[pin.toInt()] = -1; // Ensure a message back when start listen happens.
			}
			else if (inputString.substring(6, 10) == "srla") { // Start Listen Analog Pin
				String pin = inputString.substring(11);
				analogPinListening[pin.toInt()] = true;
				analogPinListenedValue[pin.toInt()] = -1; // Ensure a message back when start listen happens.
			}
			else if (inputString.substring(6, 10) == "spla") { // Stop Listen Analog Pin
				String pin = inputString.substring(11);
				analogPinListening[pin.toInt()] = false;
				analogPinListenedValue[pin.toInt()] = -1; // Ensure a message back when start listen happens.
			}
			else {
				msgRecognized = false; // this sketch doesn't know other messages in this case command is ko (not ok)
			}

			sendReplyMessage(msgRecognized);
		}

		// clear the string:
		inputString = "";
		stringComplete = false;
	}

	sendListenMessages();
}

void ArdulinkMessageReceiver::getInput() {
	while (Serial.available() && !stringComplete) {
		// get the new byte:
		char inChar = (char)Serial.read();
		// add it to the inputString:
		inputString += inChar;
		// if the incoming character is a newline, set a flag
		// so the main loop can do something about it:
		if (inChar == '\n') {
			stringComplete = true;
		}
	}
}

void ArdulinkMessageReceiver::sendReplyMessage(bool validMessage) {
	// Prepare reply message if caller supply a message id
	int idPosition = inputString.indexOf("?id=");
	if (idPosition != -1) {
		String id = inputString.substring(idPosition + 4);
		// print the reply
		Serial.print("alp://rply/");
		if (validMessage) { // this sketch doesn't know other messages in this case command is ko (not ok)
			Serial.print("ok?id=");
		}
		else {
			Serial.print("ko?id=");
		}
		Serial.print(id);
		Serial.print('\n'); // End of Message
		Serial.flush();
	}
}

void ArdulinkMessageReceiver::sendListenMessages() {
	int index = 0;
	for (index = 0; index < numDigitalPins; index++) {
		if (digitalPinListening[index] == true) {
			int value = digitalRead(index);
			if (value != digitalPinListenedValue[index]) {
				digitalPinListenedValue[index] = value;
				Serial.print("alp://dred/");
				Serial.print(index);
				Serial.print("/");
				Serial.print(value);
				Serial.print('\n'); // End of Message
				Serial.flush();
			}
		}
	}
	for (index = firstAnalogPinNum; index < numAnalogPins; index++) {
		if (analogPinListening[index] == true) {
			int value = highPrecisionAnalogRead(index);
			if (value != analogPinListenedValue[index]) {
				analogPinListenedValue[index] = value;
				Serial.print("alp://ared/");
				Serial.print(index);
				Serial.print("/");
				Serial.print(value);
				Serial.print('\n'); // End of Message
				Serial.flush();
			}
		}
	}
}

int ArdulinkMessageReceiver::highPrecisionAnalogRead(int pin) {
	int value1 = analogRead(pin);
	int value2 = analogRead(pin);
	int value3 = analogRead(pin);
	int value4 = analogRead(pin);

	int retvalue = (value1 + value2 + value3 + value4) / 4;
}