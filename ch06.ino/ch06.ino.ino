#include <TimedAction.h>
#include <ctype.h>

#include <math.h>
#include <SoftwareSerial.h>
boolean flag = false;
const int redLED = 4;
const int blueLED = 5;
const int greenLED = 6;
const int button = 8;
boolean item = false;
boolean var = false;
String id = "";
int stateButton;
int previous = LOW;
long debounce = 200;
long time = 0;
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;

TimedAction timedAction = TimedAction(1500,Lwrite);
//TimedAction tcheck = TimedAction(1000,checkLeader);
SoftwareSerial xbee(2,3);
String identity = "";
boolean lastButton = LOW;
boolean currentButton = LOW;

String getIdentity() {
  String s;

  // Enter configuration mode - Should return "OK" when successful.
  delay(1000);    // MUST BE 1000
  xbee.write("+++");
  delay(1000);    // MUST BE 1000
  xbee.write("\r");
  delay(100);

  // Get the OK and clear it out.
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");

  // Send "ATNI" command to get the NI value of xBee.
  xbee.write("ATNI\r");
  delay(100);
  while (xbee.available() > 0) {
      s += char(xbee.read());
  }
  delay(100);

  // Exit configuration mode
  xbee.write("ATCN\r");
  delay(1000);

  // Flush Serial Buffer Before Start
  while (xbee.available() > 0) {
    Serial.print(char(xbee.read()));
  }
  Serial.println("");
  delay(100);

  return s;
}
void serialEvent() {
  if (xbee.available()) {
    // get the new byte:
    char inChar = (char)xbee.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
void setup() {
  // put your setup code here, to run once:
   xbee.begin(9600);
   Serial.begin(9600);
   identity = getIdentity();
   pinMode(redLED, OUTPUT);
   pinMode(blueLED,OUTPUT);
   pinMode(greenLED,OUTPUT);
   pinMode(button, INPUT);
   digitalWrite(redLED,HIGH);
   delay(2000);
   digitalWrite(greenLED,HIGH);
   delay(2000);
   digitalWrite(blueLED,HIGH);
   delay(2000);
   digitalWrite(redLED,LOW);
   digitalWrite(greenLED,LOW);
   digitalWrite(blueLED,LOW);
  // while(!Serial);
  Serial.write(identity.c_str());
   Serialread();
   //Xbeeread();  
  xbee.println(identity.c_str());
   //Xbeeread();
    inputString.reserve(200);
 
}

void Serialread(){
  String m = "";
  if(Serial.available()){
   m += char(Serial.read());
   
   //Serial.println(Serial.read());
   delay(1);
  // Serial.flush();
   
   //delay(1);
   Serial.println(m);}
}
void Xbeeread(){
   
    if(xbee.available()){
     String m = "";
   // while(xbee.available()>0){
  //  m += char(xbee.read());
   // delay(1);
   // }
    
    Serial.write(xbee.read());
   
    }
}
String Xbee(){
  String m = "";
  if(xbee.available()){
     m += char(xbee.read());
  }
  return m;
}

boolean XBEE(){
  
  String m = "";
  if(xbee.available()){
     m += char(xbee.read());
    if(m.equals("CLR")){item = true; return true;}
  else if(m.equals("INF")){var = true;return true;}
  else return false;
  }
  
}
boolean checkLeader(){
  if (xbee.available()){
    String msg = "";
   msg += char(xbee.read());
   if(msg.equals("Leader\n"))
   return true;
   else return false;
  }
}
void Lwrite(){
  Serial.write("Leader\n");
}
void clr(){
  Serial.write("CLR\n");
}

void loop() {
  int g;
    
    String mId = Xbee();
    
    g = mId[0];
    for (int i = 0; i< sizeof(mId) ; i++){
      if(g > mId[i])
       g = mId[i]; 
    }
    
  // put your main code here, to run repeatedly:

 /* while(xbee.available()>0){
    Serial.print((char)xbee.read());
  }*/
   //currentButton = debounce(lastButton);
   stateButton = digitalRead(button);
   //Serial.println(identity.toInt());
   if (identity.toInt() == 1 ) {
    //if(flag == false){
    digitalWrite(blueLED,HIGH);    
    digitalWrite(greenLED,HIGH);
    delay(1000);
    timedAction.check();
    
   // }
    
    if(stateButton == HIGH && previous == LOW && millis() - time > debounce){
      xbee.println("CLR");
      Serial.write("CLR\n");
      flag = true;
      //attachInterrupt(digitalPinToInterrupt(button), clr, CHANGE);
        time = millis();
        //timedAction.check();
    }
    previous = stateButton;
   }
   else if(identity.toInt() != 1 ) {
    
   if(flag == false){
    
    digitalWrite(greenLED,HIGH);}
   // if(digitalRead(button) == HIGH){
    if(stateButton == HIGH && previous == LOW && millis() - time > debounce){
      digitalWrite(redLED,HIGH);
      digitalWrite(greenLED,LOW);flag = true; 
      Serial.write("INF\n"); 
      xbee.println("INF");
     // lastButton = currentButton;
     time = millis();
    }
    
    previous = stateButton;
    serialEvent();
    if(stringComplete){
      if(inputString.equals("CLR")){
         digitalWrite(redLED,LOW);
         digitalWrite(greenLED,HIGH);
         delay(3000);
         Serial.println("ITEM");
      }
      else if (inputString.equals("INF")){
          digitalWrite(redLED,HIGH);
          digitalWrite(greenLED,LOW);
          Serial.println("var");
      }
    }
  /*if(XBEE){
    Serial.println("XBEE");
    if(item){
    digitalWrite(redLED,LOW);
    digitalWrite(greenLED,HIGH);
    delay(3000);
    Serial.println("ITEM");
  }
  else if(var){
     
     digitalWrite(redLED,HIGH);
    digitalWrite(greenLED,LOW);
    Serial.println("var");
  }
   }*/

   }

    
   }
   
    
   





