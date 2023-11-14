#include "Joystick.h"
#include "joy.h"
#include "state.h"
#include "eeprom.h"


Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,
                   JOYSTICK_TYPE_JOYSTICK, 12, 0,
                   true, true, true, true, true, false,
                   false, false, false, true, false);


void setupJoystick() {

  Joystick.setXAxisRange(0, AXIS_MAX); // throttle 1
  Joystick.setYAxisRange(0, AXIS_MAX); // throttle 2
  Joystick.setZAxisRange(0, AXIS_MAX); // flaps
  Joystick.setRxAxisRange(0, AXIS_MAX); // reverse 1
  Joystick.setRyAxisRange(0, AXIS_MAX); // reverse 2
  Joystick.setBrakeRange(0, AXIS_MAX); // speed brake

  Joystick.begin();
}

void loopJoystick() {

}

void setJoystickAxis(uint8_t axis, uint16_t value) {
  switch (axis) {
    case 0: // speed brake
      Joystick.setBrake(value);
      break;
    case 1:  // Thr1
      Joystick.setXAxis(value);
      break;
    case 2:  // Thr2
      Joystick.setYAxis(value);
      break;
    case 3:  // Flaps
      Joystick.setZAxis(value);
      break;
    case 4:  // Reverse 1
      Joystick.setRxAxis(AXIS_MAX - value);  // reversed in HW
      break;
    case 5:  // Reverse 2
      Joystick.setRyAxis(value);
      break;
  }
}

void setJoystickButton(uint8_t button, uint8_t value) {
  Joystick.setButton(button, value);
}

void setCalibrateAxis(uint8_t axis, bool calibrate) {
  if (axis == ALL_AXIS) {
    for (int i = 0; i < NUMBER_OF_ANALOG_INPUTS; i++) {
      axisState *a = getAxis(i);
      a->calibrating = calibrate;
      if (calibrate) {
        a->minValue = AXIS_MAX;
        a->maxValue = 0;
      }
    }
  } else {
    axisState *a = getAxis(axis);
    a->calibrating = calibrate;
    if (calibrate) {
      a->minValue = AXIS_MAX;
      a->maxValue = 0;
    }
  }
  // write to eeprom on calibration end
  if (!calibrate) {
    writeCalibrationToEeprom();
  }
}