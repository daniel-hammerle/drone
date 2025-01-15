#include "packets.h"
#include <ArduinoBLE.h>

BLEService droneService("1a5b24de-3c1c-46cd-89a7-bfcd163d7019"); // Custom UUID for the service;
BLECharacteristic commandCharacteristic("87654321-4321-6789-4321-abcdef012345", BLEWrite, sizeof(Packet));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(850);
  
  if (!BLE.begin()) {
    panic("Failed to intialize bluetooth");
  }

  Serial.println("Initialized bluetooth");
  
  BLE.setLocalName("Drone");
  BLE.setAdvertisedService(droneService);

 // Add characteristics to the service
  droneService.addCharacteristic(commandCharacteristic);
  // Add the service
  
  BLE.addService(droneService);
  // Start advertising
  BLE.advertise();
  /*
  Serial.println("Advertising...");
  */
}

void panic(const char* message) {
  //TODO when adding actual motor controls make em turn off
  Serial.println(message);
  for(;;);
}

void handleConnection(BLEDevice& central);

struct DroneState {
  float x;
  float y;
  float z;
  float turn;
};

void loop() {
  BLEDevice central;
  findConnection(central);
  droneLoop(central);
}


void findConnection(BLEDevice& central) {
  Serial.println("Waiting for connection");
  for(;;) {
    //listen to bluetooth
    central = BLE.central();
    //if we have a ble connection handle it 
    if (central) {
      Serial.println("Connection found");
      //central.connect();
      Serial.println("Connected");
      break;
    }
  }
}

void handlePacket(Packet& packet, struct DroneState& state) {
  //simply log the packet for now so we see if the bluetooth connection worked
  switch(packet.type) {
  case INS_MOVE:
    state.x = ((float) packet.u.movePacket.dx) / 10000.0;
    state.y = ((float) packet.u.movePacket.dy) / 10000.0;
    state.z = ((float) packet.u.movePacket.dz) / 10000.0;
    break;
  }
}

void droneLoop(BLEDevice& central) {
  DroneState state;

  for(;;) {

    if (!central.connected()) {
      //connection lost 
      //TODO recovery mode
      Serial.println("Disconnected unexpectely");
      return;
    }


    // Check if data is available to read
    if (commandCharacteristic.written()) {
      Serial.println("recieved");
      //read and validate a packet
      Packet p;

      commandCharacteristic.readValue((int8_t*)&p, sizeof(p));

      if (validatePacket(p)) {
        //gracefully ignore invalid packet
        Serial.println("Invalid packet recievewd");
        continue;
      }
      

      if (p.type == INS_KILL) {
        painc("Killed");
      }

      //handle the packet
      handlePacket(p, state);
    }
  }

  
}



