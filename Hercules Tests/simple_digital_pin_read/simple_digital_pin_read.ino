
//int ledPin = 13; // LED connected to digital pin 13
int inPin = 2;   // pushbutton connected to digital pin 7
int val = 0;     // variable to store the read value

void setup()
{
  Serial.begin(9600);
//  pinMode(ledPin, OUTPUT);      // sets the digital pin 13 as output
  pinMode(inPin, INPUT);      // sets the digital pin 7 as input
//  attachInterrupt(digitalPinToInterrupt(inPin), derp, LOW);
//  digitalWrite(inPin, HIGH);
  Serial.println("DERP");
}

void loop()
{
//  Serial.print("DERP");
  int newVal = digitalRead(inPin);   // read the input pin

//  Serial.println((newVal == HIGH)? "HIGH" : "LOW");
  Serial.println(newVal);
  
  delay(20);
}
