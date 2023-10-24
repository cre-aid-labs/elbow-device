#ifndef DEVICE_CONTROLLER_H
#define DEVICE_CONTROLLER_H

#include "motorcontroller.h"

class HandDeviceController {
  LAController* motorctrl;
  void controlLoop();
  void (*curfunc)();
  public:
  static void controlLoopStaticWrapper(void* obj);
  void emergencyStop();
  void cpm(int reps);
  void fullFlex(int spd);
  void fullExtend(int spd);
  HandDeviceController(LAController* motorctrl);
};

#endif