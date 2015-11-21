#include <XBee.h>
#include <math.h>
#include <SoftwareSerial.h>

// Looping Variables
boolean isLeader;
int leaderID;
int final_id;

//identity of this node
uint8_t identity = '3';

boolean timeout_flag = false;
int timeout_count = 0;

int checkLeader_timer = 0;
int election_timer = 0;
int leader_timer = 0;

int election_timeout = 8;
int checkLeader_timeout = 8;
int leader_timeout = 5;

bool expireFlag = true; //new



XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t dbCommand[] = {'D','B'};
AtCommandRequest atRequest = AtCommandRequest(dbCommand);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();

#define ssRX 2
#define ssTX 3
SoftwareSerial nss(ssRX, ssTX);
SoftwareSerial xbeeSerial(2,3);


int red=4;
int blue=6;
int green=5;
int switchState = 0;

void setup (){
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  isLeader = false;
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing transmitter...");
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(red, OUTPUT);

  pinMode(8, INPUT);
  digitalWrite(green,HIGH);
}

void processResponse(){
       for (int i= 0; i < rx.getDataLength(); i++){
        if (rx.getData()[i] == 'Y' )
        {   
           for (int j= i; j < i+2; j++)   
           {
              uint8_t data = rx.getData()[j];
              
              if (data == '1')
              {
                delay(100);
//                id_list[1] = 1;
//                id_time[1] = millis();
//                Serial.write(data);
                Serial.println("Received id 1");
                digitalWrite(green, HIGH);
                digitalWrite(red, LOW);
                digitalWrite(blue, LOW);
//                Serial.print(id_list[1]);
//                Serial.print(id_time[1]);
                //Serial.write(millis());
              }
              if (data == '2')
              {
//                id_list[2] = 2;
//                id_time[2] = millis();
//                Serial.write(data);
                Serial.println("Received id 2");
                digitalWrite(green, LOW);
                digitalWrite(red, LOW);
                digitalWrite(blue, HIGH);
//                Serial.write(id_list[2]);
//                Serial.write(id_time[2]);
              }
              if (data == '3')
              {
//                id_list[3] = 3;
//                id_time[3] = millis();
//                Serial.write(data);
                Serial.println("Received id 3");
                digitalWrite(green, LOW);
                digitalWrite(red, LOW);
                digitalWrite(blue, LOW);
//                Serial.print(id_list[3]);
//                Serial.print(id_time[3]);
              }
              if (data == '4')
              {
//                id_list[4] = 4;
//                id_time[4] = millis();
//                Serial.write(data);
                Serial.println("Received id 4");
//                Serial.print(id_list[4]);
//                Serial.print(id_time[4]);
              }
           }
           break;
        }
      }
//  if (xbee.getResponse().isAvailable()) {
//      // got something
//      //xbee conntected
//      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
//        // got a zb rx packet
//        
//        // now fill our zb rx class
//        xbee.getResponse().getZBRxResponse(rx);
//        if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
//           // the sender got an ACK
//            Serial.println("packet acknowledged");
//        } else {
//          Serial.println("packet not acknowledged");
//        }
//        int id = rx.getData()[0];
//        Serial.println("GET ID : " + String(id));
//        if (id == leaderID) {
//          checkLeader_timer = 0;
//        } else {
//          election(id);
//        }
//      }
//    } else if (xbee.getResponse().isError()) {
//      Serial.print("error code:");
//      Serial.println(xbee.getResponse().getErrorCode());
//    }
}


//rebroadcast leader id
void broadcastMsg(int id) {
  uint8_t value = (uint8_t)id;
  Serial.println("here2" + String(value));
  uint8_t payload[] = {value};

  // Specify the address of the remote XBee (this is the SH + SL)
  XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  xbee.send(zbTx);
}

void leaderBroadcast() {
  uint8_t payload[] = {identity};
  Serial.println("here1" + String(identity));

  // Specify the address of the remote XBee (this is the SH + SL)
      XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  xbee.send(zbTx);
}

boolean checkLeaderExpire() {
  if (checkLeader_timer >= checkLeader_timeout || leaderID == -1) {
    leaderID = -1;
    return true;
  } else {
    return false;
  }
}

boolean checkElectionTimeOut() {
  if (timeout_flag) {
    if (timeout_count < 3) {
      timeout_count++;
    } else {
      timeout_flag = false;
      timeout_count = 0;
    }
  }
  return timeout_flag;
}

void election(int id) {
  Serial.println("Electing...");
  if (checkElectionTimeOut()) {
    return;
  }
  leaderID = -1;
  if (id > final_id) {
    final_id = id;
    election_timer = 0;
    broadcastMsg(final_id);
  } else {
    if (election_timer >= election_timeout){
      election_timer = 0;
      timeout_count = 0;
      timeout_flag = true;
      leaderID = final_id;
    } else {
      election_timer++;
      broadcastMsg(final_id);
    }
  }
}

void loop(){
   /*Send Message*/
      uint8_t payload[] = {'Y', identity};

      // Specify the address of the remote XBee (this is the SH + SL)
      XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x0000FFFF);
      // Create a TX Request
      ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
      // Send your request
      xbee.send(zbTx);

  delay(300);

  /*Receive Data*/
  xbee.getResponse().getZBRxResponse(rx);
        for (int i= 0; i < rx.getDataLength(); i++){
        if (rx.getData()[i] == 'Y' )
        {   
           for (int j= i; j < i+2; j++)   
           {
              uint8_t data = rx.getData()[j];
              
              if (data == '1')
              {
                delay(100);
//                id_list[1] = 1;
//                id_time[1] = millis();
//                Serial.write(data);
                Serial.println("Received id 1");
                digitalWrite(green, HIGH);
                digitalWrite(red, LOW);
                digitalWrite(blue, LOW);
//                Serial.print(id_list[1]);
//                Serial.print(id_time[1]);
                //Serial.write(millis());
              }
              if (data == '2')
              {
//                id_list[2] = 2;
//                id_time[2] = millis();
//                Serial.write(data);
                Serial.println("Received id 2");
                digitalWrite(green, LOW);
                digitalWrite(red, LOW);
                digitalWrite(blue, HIGH);
//                Serial.write(id_list[2]);
//                Serial.write(id_time[2]);
              }
              if (data == '3')
              {
//                id_list[3] = 3;
//                id_time[3] = millis();
//                Serial.write(data);
                Serial.println("Received id 3");
//                Serial.print(id_list[3]);
//                Serial.print(id_time[3]);
              }
              if (data == '4')
              {
//                id_list[4] = 4;
//                id_time[4] = millis();
//                Serial.write(data);
                Serial.println("Received id 4");
//                Serial.print(id_list[4]);
//                Serial.print(id_time[4]);
              }
           }
           break;
        }
      }
//  xbee.readPacket();
//  processResponse();
//  if (leaderID == identity) {
//    if (leader_timer == leader_timeout) {
//      leader_timer = 0;
//      leaderBroadcast();
//    } else {
//      leader_timer++;
//    }
//  } else if(checkLeader_timer >= checkLeader_timeout){
//    //fix the bug when remove the rest Arduino but leave one
//    checkLeader_timer = 0;
//    Serial.println("Leader ID : "+String(leaderID));
//  }else {
//   checkLeader_timer++;
//   Serial.println("checkLeader_timer : " + String(checkLeader_timer) + "election_timer : " +  election_timer);
//   if (checkLeaderExpire()) {
//     if (election_timer < election_timeout) {
////          Serial.println("here6");
//       broadcastMsg(final_id);
//       election_timer++;
//     } else {
//       // election_timer = 0
//       leaderID = final_id;
//     }
//   }
// }
}
