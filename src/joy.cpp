#include "Joystick.h"
#include "joy.h"


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 10, 0,
                   true, true, true, true, true, false,
                   false, false, false, true, false);


void setupJoystick() {

  Joystick.setXAxisRange(0, 4096); // throttle 1
  Joystick.setYAxisRange(0, 4096); // throttle 2
  Joystick.setZAxisRange(0, 4096); // flaps
  Joystick.setRxAxisRange(0, 4096); // reverse 1
  Joystick.setRyAxisRange(0, 4096); // reverse 2
  Joystick.setBrakeRange(0, 4096); // speed brake

  Joystick.begin();


}

void loopJoystick() {

}

void setJoystickAxis(uint8_t axis, uint16_t value) {
  switch (axis) {
    case 0:
      Joystick.setXAxis(value);
      break;
    case 1:
      Joystick.setYAxis(value);
      break;
    case 2:
      Joystick.setZAxis(value);
      break;
    case 3:
      Joystick.setRxAxis(value);
      break;
    case 4:
      Joystick.setRyAxis(value);
      break;
    case 5:
      Joystick.setBrake(value);
      break;
  }
}

void setJoystickButton(uint8_t button, uint8_t value) {
  Joystick.setButton(button, value);
}