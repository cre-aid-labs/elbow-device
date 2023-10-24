#include "status_led.h"
#include <arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void StatusLED::ledLoop() {
  while(true) {
    if(status_changed) {
      status_changed = false;
      vTaskDelay(100/portTICK_PERIOD_MS);
    }
    while(!(status_changed)) {
      curfunc(this);
    }
  }
}

void StatusLED::ledLoopStaticWrapper(void* obj) {
  StatusLED* obj_led = (StatusLED*) obj;
  obj_led -> ledLoop();
}

StatusLED::StatusLED(int led_pin) {
  this->led_pin = led_pin;
  this->status_led_task = NULL;
  this->status_changed = false;
  this->curfunc = NULL;
  setStatus(STATUS_INIT);
}

void StatusLED::setStatus(int s) {
  status_changed = true;
  switch(s) {
    case STATUS_INIT:
      curfunc = noconn;
      break;
    case STATUS_WARN:
      curfunc = warning;
      break;
    case STATUS_ERR:
      curfunc = error;
      break;
    case STATUS_GOOD:
      curfunc = nominal;
      break;
    case STATUS_NOCO:
      curfunc = noconn;
      break;
    default:
      curfunc = error;
      break;
  }
}

void StatusLED::initLED(StatusLED* obj_led) {
  xTaskCreate(
    StatusLED::ledLoopStaticWrapper,
    "led loop",
    10000,
    (void*) obj_led,
    1,
    obj_led -> status_led_task
  );
  vTaskDelay(500/portTICK_PERIOD_MS);
}

void StatusLED::init(StatusLED* obj_led) {
  neopixelWrite(obj_led -> led_pin, 100, 100, 100);
}

void StatusLED::error(StatusLED* obj_led) {
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(500/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 255, 0, 0);
  vTaskDelay(500/portTICK_PERIOD_MS);
}

void StatusLED::warning(StatusLED* obj_led) {
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(700/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 255, 255, 0);
  vTaskDelay(200/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(300/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 255, 255, 0);
  vTaskDelay(100/portTICK_PERIOD_MS);
}

void StatusLED::nominal(StatusLED* obj_led) {
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(700/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 0, 255, 0);
  vTaskDelay(200/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(300/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 0, 255, 0);
  vTaskDelay(100/portTICK_PERIOD_MS);
}

void StatusLED::noconn(StatusLED* obj_led) {
  neopixelWrite(obj_led -> led_pin, 0, 0, 0);
  vTaskDelay(500/portTICK_PERIOD_MS);
  neopixelWrite(obj_led -> led_pin, 0, 0, 255);
  vTaskDelay(500/portTICK_PERIOD_MS);
}
