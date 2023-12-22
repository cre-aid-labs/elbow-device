#ifndef ELBOW_BRACE_H
#define ELBOW_BRACE_H
#include "motorcontroller.h"
#include "AS5600.h"
#include "hexobt.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ANGLE_CTRL_THRESHOLD 0.5

class ElbowBrace {
  AS5600* elbow_encoder = NULL;
  HexoBT* hexobt = NULL;
  LAController* controller = NULL;
  TaskHandle_t elbow_task;
  TaskHandle_t elbow_serial_task;
  int enc_sda;
  int enc_scl;
  int enc_dir;

  bool angle_control = false;
  bool rom_limit_enabled = true;
  float set_point = 0.0;
  float flex_rom_limit = 110.0;
  float ext_rom_limit = 0.0;

  float omega = 0.0;
  float theta = 0.0;

  void controlLoop();
  void serialTransmitLoop();
  public:
  static void controlLoopWrapper(void* obj);
  static void serialTransmitLoopWrapper(void* obj);
  void setController(LAController* controller);
  void initDevice();
  void setReference();
  void setAngle(float angle);
  void moveByAngle(float angle);
  void setFlexLimit(float angle);
  void setExtLimit(float angle);
  void setFlexLimitAtPosition();
  void setExtLimitAtPosition();
  float getAngle();
  bool isAngleControlEnabled();
  void enableAngleControl();
  void disableAngleControl();
  void enableROMLimits();
  void flex();
  void extend();
  ElbowBrace(int enc_sda, int enc_scl, int enc_dir, HexoBT* hexobt);
};

#endif