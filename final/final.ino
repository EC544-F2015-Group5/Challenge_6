#include <XBee.h>
#include <math.h>
#include <SoftwareSerial.h>


SoftwareSerial xbee(2,3); // Rx, Tx

// Declare port
int red = 4;
int green = 5;
int blue = 6;
int switchState = 0;


// Declare Variables
boolean isLeader;
int leaderID;
int finalID;
int id;

// Status of each xbee(1:Infected 0:Cured)
int status = 0;

//Declare time variables
boolean timeoutFlag = false;
int timeoutCount = 0;
int checkingTime = 0;
int electionTime = 0;
int leaderTime = 0;
int election_timeout = 2;
int checkLeader_timeout = 5;
int leader_timeout = 1;

//AT command get id
int getIdentity() {
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

  return s.toInt();
}

void setup() {
  xbee.begin(9600);
  Serial.begin(9600);
  isLeader = false;

  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(8, INPUT);
  digitalWrite(green,HIGH);

  id = getIdentity();
  finalID = id;
  leaderID = -1;
  Serial.println("My ID : "+ String(id));
  xbee.flush();
  Serial.println("Setup Complete");
}

//Response function
void processResponse(){
  if (xbee.available()) {
    String msg = readTheMsg();
    String info = msg.substring(msg.indexOf(':') + 1);
    int id = msg.substring(0, msg.indexOf(':')).toInt();
    if (info == "Infection") {
      infection();
    } else if (info == "Curing") {
      cure();
      checkingTime = 0;
    } else {
      Serial.println(info);
      String othersStatus = info.substring(info.indexOf(':') + 1);
      info = info.substring(0, info.indexOf(':'));
      if (othersStatus == "1") {
        status = 1;
      } 
      isLeader = false;
      checkingTime = 0;
      if (id == leaderID) {
        if (leaderID == id) {
          leaderBroadcast();
        } else {
          Serial.println("leader is alive");
        }
      } else {
        election(id);
      }
    }
  } else {
    checkLeader();
  }
}

//Infection function
void infection(){
  if(!isLeader){
    digitalWrite(red, HIGH);
    digitalWrite(green, LOW);
    status = 1;
  }
}

//Cure function
void cure(){
  digitalWrite(red, LOW);
  digitalWrite(green, HIGH);
  status = 0;
}


//ReadMessage Function
String readTheMsg() {
  String msg  = "";
  while(xbee.available() > 0) {
    char c = char(xbee.read());
    if (c == '\n') {
      break;
    }
    msg += c;
  }
  Serial.println(msg);
  return msg;
}

//Broadcast Template Leader Function
void broadcastMsg(int id) {
  xbee.print(String(id) + ":Leader:" + String(status) + "\n");
  Serial.println("Temp Leader :" + String(id) + " Status : " + String(status));
}

//Broadcast Final Leader Function
void leaderBroadcast() {
  xbee.print(String(id)+ ":Leader:2\n");
  Serial.println("New Leader :" + String(leaderID));
  digitalWrite(blue, HIGH);
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  isLeader = true;
}

//Check Election Time
boolean checkElectionTimeOut() {
  if (timeoutFlag) {
    if (timeoutCount < 2) {
      timeoutCount++;
    } else {
      timeoutFlag = false;
      timeoutCount = 0;
    }
  }
  return timeoutFlag;
}

//Election Process
void election(int id) {
  Serial.println("Electing...");
  digitalWrite(blue, LOW);
  if (checkElectionTimeOut()) {
    Serial.println("election over");
    return;
  }
  leaderID = -1;
  if (id > finalID) {
    finalID = id;
    electionTime = 0;
    broadcastMsg(finalID);
    Serial.println("new candidate");
  } else {
    if (electionTime >= election_timeout){
      electionTime = 0;
      timeoutCount = 0;
      timeoutFlag = true;
      assignLeader();
      Serial.println("election timeout");
    } else {
      electionTime++;
      broadcastMsg(finalID);
      Serial.println("election continue" + String(electionTime));
    }
  }
}

void assignLeader() {
  leaderID = finalID;
  finalID = id;
  Serial.println("Leader ID : "  + String(leaderID));
  if (leaderID == id) {
    status = 0;
    digitalWrite(green, HIGH);
  } else {
    if (status == 1) {
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
    } else {
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
    }
  }
}

//Check Current Leader Removed or not
void checkLeader() {
  if (leaderID == id) {
    if (leaderTime >= leader_timeout) {
      leaderTime = 0;
      leaderBroadcast();
    } else {
      leaderTime++;
    }
  } else if(checkingTime >= checkLeader_timeout){
        //fix the bug when remove the rest Arduino but leave one
        checkingTime = 0;
        leaderID = -1;
        broadcastMsg(id);
        Serial.println("Leader "+String(leaderID) + " is dead.");
    }else {
      Serial.println("checkingTime : " + String(checkingTime) + "electionTime : " +  String(electionTime));
      if (leaderID == -1) {
        checkingTime = 0;
        if (electionTime < election_timeout) {
          broadcastMsg(finalID);
          electionTime++;
        } else {
          electionTime = 0;
          assignLeader();
        }
      } else {
         checkingTime++;
      }
    }
}


void loop(){
  switchState = digitalRead(8);
  if(switchState == 1){
    if(isLeader){
        xbee.print(String(id) + ":Curing\n");
        status = 0;
        Serial.println("Healing Potion is coming  !!!");
    }else{
        xbee.print(String(id) + ":Infection\n");
        Serial.println("Virus is coming!!!");
        digitalWrite(red, HIGH);
        digitalWrite(green, LOW);
        status = 1;
    }
  }
  processResponse();
  delay(1000);
}

