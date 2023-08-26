#include "SerialUSB.h"

SerialUSB_ SerialUSB;

void setupUsbIn() {
    USB_Begin();
    SerialUSB.begin(115200);
}

void loopUsbIn() {
  if (SerialUSB.available()){
    //lastChar = SerialUSB.read();
  }
}

bool usbSerialConnected() {
    return SerialUSB;
}