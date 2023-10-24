#include "device_controller.h"

HandDeviceController::HandDeviceController(LAController* motorctrl) {
  this -> motorctrl = motorctrl;
}