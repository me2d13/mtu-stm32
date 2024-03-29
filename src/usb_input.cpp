#include "SerialUSB.h"
#include <ArduinoJson.h>
#include "state.h"
#include "motors.h"
#include "trim.h"

SerialUSB_ SerialUSB;
StaticJsonDocument<4096> doc;

// ./SerialSend.exe /devnum 5 /noscan "{\"command\":\"motorPos\", \"index\": 0,\"relative\": 10}"
// ./SerialSend.exe /devnum 5 "{\"command\":\"getCalibrationInfo\"}"
// {"command":"hello"}
// {"command":"getCalibrationInfo"}
// {"command":"getState"}
// {"command":"setPositions", "positions": { "elevatorTrim": 23.6}}
// {"command":"setPositions","positions":{"elevatorTrim":30.5, "speedBrake": 20, "throttle1": 30, "throttle2": 40}}

/*

COM5
----
Hardware ID: USB VID:PID=0483:5711 SER=005384424D34 LOCATION=1-6.1:x.1
Description: Sériové zařízení USB (COM5)

*/

void setupUsbIn() {
    USB_Begin();
    SerialUSB.begin(115200);
}

void sendCalibrationInfo() {
  doc.clear();
  JsonArray data = doc.createNestedArray("axis");
  for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
    JsonObject axisNode = data.createNestedObject();
    axisState *axis = getAxis(i);
    axisNode["index"] = i;
    axisNode["min"] = axis->minValue;
    axisNode["max"] = axis->maxValue;
  }
  serializeJson(doc, SerialUSB);
  SerialUSB.println(""); // new line char
}

void sendStateInfo() {
  doc.clear();
  JsonArray axisNodes = doc.createNestedArray("axis");
  for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
    JsonObject axisNode = axisNodes.createNestedObject();
    axisState *axis = getAxis(i);
    axisNode["index"] = i;
    axisNode["value"] = axis->value;
    axisNode["rawValue"] = axis->rawValue;
  }
  JsonArray buttonNodes = doc.createNestedArray("buttons");
  for (int i = 0; i < NUMBER_OF_BUTTONS; i++) {
    JsonObject buttonNode = buttonNodes.createNestedObject();
    buttonState *buttons = getButtons();
    buttonNode["index"] = i;
    buttonNode["value"] = buttons[i].value;
  }
  serializeJson(doc, SerialUSB);
  SerialUSB.println(""); // new line char
}

void processSetPosition() {
  JsonObject positions = doc["positions"];
  if (positions.containsKey("elevatorTrim")) {
    float elevatorTrim = positions["elevatorTrim"];
    setElevatorTrim(elevatorTrim);
  }
  if (positions.containsKey("speedBrake")) {
    float value = positions["speedBrake"];
    moveMotorBySensor(0, value);
  }
  if (positions.containsKey("throttle1")) {
    float value = positions["throttle1"];
    moveMotorBySensor(1, value);
  }
  if (positions.containsKey("throttle2")) {
    float value = positions["throttle2"];
    moveMotorBySensor(2, value);
  }
}

void processCommand() {
    String command = doc["command"];
if (command == "hello") {
      SerialUSB.println("mtu hello");
    } else if (command == "getCalibrationInfo") {
      sendCalibrationInfo();
    } else if (command == "setPositions") {
      processSetPosition();
    } else if (command == "getState") {
      sendStateInfo();
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