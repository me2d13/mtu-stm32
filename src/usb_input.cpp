#include "SerialUSB.h"
#include <ArduinoJson.h>
#include "state.h"
#include "motors.h"

SerialUSB_ SerialUSB;
StaticJsonDocument<200> doc;

// ./SerialSend.exe /devnum 11 /noscan "{\"command\":\"motorPos\", \"index\": 0,\"relative\": 10}"

void setupUsbIn() {
    USB_Begin();
    SerialUSB.begin(115200);
}

void processCommand() {
    String command = doc["command"];
    if (command == "motorPos") {
        int index = doc["index"];
        int relative = doc["relative"];
        motorMoveRelation(index, relative);
    }
}

void loopUsbIn() {
  if (SerialUSB && SerialUSB.available()){
    //lastChar = SerialUSB.read();
    String message = SerialUSB.readString();
    setLastMessage(message);
    
    DeserializationError error = deserializeJson(doc, message);
    if (error) {
      setLastMessage("ERR: " + String(error.c_str()) + "\n" + message);
    } else {
      processCommand();
    }
  }
}

bool usbSerialConnected() {
    return SerialUSB;
}