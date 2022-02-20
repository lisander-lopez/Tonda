#include <Arduino.h>
#include <FlexCAN_T4.h>
#include "LedLibrary.h"

#define BTSERIAL Serial2

const String BT_MAC_ADDRESS = "9C04A015D800";
const int CONNECT_RETRY_COUNT = 3;

LedLibrary LedLibrary(&BTSERIAL);

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can0; // ODB Port "FCAN"

// Which byte of the "frame byte" is responsible for...
byte leftTurn = 0x4;
byte rightTurn = 0x8;

// Index of data buffer that contains data for left turn status
int whichByte = 4;

// ID of actual CAN Frame that containers left turn status
uint32_t leftTurnFrameID = 0x326;

void processFCANMessage(const CAN_message_t &msg) {
  if( msg.id == leftTurnFrameID){
    byte dataByteFrame = msg.buf[whichByte - 1];
    if(dataByteFrame & leftTurn){
      LedLibrary.setLeftTurn(true);
    } else {
      LedLibrary.setLeftTurn(false);
    }

    if(dataByteFrame & rightTurn){
      // Right turn!
    }
  }
}

bool connectToDevice() {
  String command = ("AT+CON" + BT_MAC_ADDRESS);
  BTSERIAL.print(command);
  BTSERIAL.flush();
  Serial.println("Connecting to device...");
  delay(1000); // Wait a second
  String response = BTSERIAL.readString();
  Serial.println(response);
  if(response.indexOf("OK+CON") != -1){
    return true;
  }
  return false;
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  BTSERIAL.begin(115200);
  Can0.begin();
  BTSERIAL.print("AT"); // Tell Cluetooth to disconnect if connected to anything!
  BTSERIAL.flush();
  delay(100);
 
  bool isConnected = false;
  while(!isConnected) {
    isConnected = connectToDevice();
    String response = (isConnected ? "Yes" : "No");
    Serial.println("Connected to Lights: " + response);
  }

  LedLibrary.begin();

  Can0.setBaudRate(500000);
  Can0.setMaxMB(16);
  Can0.enableFIFO();
  Can0.enableFIFOInterrupt();
  // Only read from these Can Frames
  Can0.setFIFOFilter(REJECT_ALL);
  Can0.setFIFOFilter(1,0x326, STD);

  Can0.onReceive(processFCANMessage);
  Can0.mailboxStatus();
}

void loop() {
  LedLibrary.tick();
  Can0.events();
  // put your main code here, to run repeatedly:
}