void setup() {
  // initialize digital pin 13 as an output.
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, INPUT);
}

// the loop function runs over and over again forever
void loop() {
  if ()
  digitalWrite(5, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(6, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(7, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
}

int ledPin1 = 5; // LED connected to digital pin 13
int ledPin2 = 6; // LED connected to digital pin 13
int ledPin3 = 7; // LED connected to digital pin 13

int inPin = 8;   // pushbutton connected to digital pin 7

void setup()
{
  pinMode(ledPin1, OUTPUT);      // sets the digital pin 13 as output
  pinMode(ledPin2, OUTPUT);      // sets the digital pin 13 as output
  pinMode(ledPin3, OUTPUT);      // sets the digital pin 13 as output 
  pinMode(inPin, INPUT);      // sets the digital pin 7 as input
}

void loop()
{
  digitalWrite(ledPin1, HIGH);    // sets the LED to the button's value
  digitalWrite(ledPin2, HIGH);    // sets the LED to the button's value
  digitalWrite(ledPin3, HIGH);    // sets the LED to the button's value
  if (digitalRead(inPin) == HIGH) {
         digitalWrite(ledPin1, LOW);    // sets the LED to the button's value
         digitalWrite(ledPin2, LOW);    // sets the LED to the button's value
         digitalWrite(ledPin3, LOW);    // sets the LED to the button's value
    }
    delay(1000);
    
}
