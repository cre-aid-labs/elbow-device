#ifndef STATUS_LED_H
#define STATUS_LED_H

#define STATUS_INIT 0x00
#define STATUS_WARN 0x02
#define STATUS_ERR  0x04
#define STATUS_GOOD 0x06
#define STATUS_NOCO 0x08

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class StatusLED {
  public:
  int led_pin;
  bool status_changed;
  void ledLoop();
  void (*curfunc)(StatusLED* obj_led);
  static void warning(StatusLED* obj_led);
  static void error(StatusLED* obj_led);
  static void nominal(StatusLED* obj_led);
  static void noconn(StatusLED* obj_led);
  static void init(StatusLED* obj_led);
  static void ledLoopStaticWrapper(void* obj);
  TaskHandle_t* status_led_task;
  static void initLED(StatusLED* obj_led);
  void setStatus(int s);
  StatusLED(int led_pin);
};

#endif
