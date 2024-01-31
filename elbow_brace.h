#ifndef ELBOW_BRACE_H
#define ELBOW_BRACE_H
#include "motorcontroller.h"
#include "AS5600.h"
#include "hexobt.h"
#include "pid.h"
#include "saved_data.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ANGLE_CTRL_THRESHOLD -0.5

class ElbowBrace {
  AS5600* elbow_encoder = NULL;
  HexoBT* hexobt = NULL;
  LAController* controller = NULL;
  //PIDController* pid = NULL;
  TaskHandle_t* elbow_task = NULL;
  TaskHandle_t* elbow_serial_task = NULL;
  BraceSettings* settings = NULL;
  int enc_sda;
  int enc_scl;
  int enc_dir;

  bool angle_control = false;
  bool rom_limit_enabled = false;
  float set_point = 0.0;
  float flex_rom_limit = 110.0;
  float ext_rom_limit = 0.0;

  float omega = 0.0;
  float theta = 0.0;

  void controlLoop();
  void serialTransmitLoop();
  public:
  PIDController* pid = NULL;
  static void controlLoopWrapper(void* obj);
  static void serialTransmitLoopWrapper(void* obj);
  void setController(LAController* controller);
  void initDevice();
  void getPreferences();
  void setReference();
  float getReference();
  void setAngle(float angle);
  void moveByAngle(float angle);
  void setFlexLimit(float angle, bool save_to_device = false);
  void setExtLimit(float angle, bool save_to_device = false);
  void setFlexLimitAtPosition(bool save_to_device = false);
  void setExtLimitAtPosition(bool save_to_device = false);
  float getAngle();
  bool isAngleControlEnabled();
  void enableAngleControl();
  void disableAngleControl();
  bool isROMLimitEnabled();
  void enableROMLimits();
  void disableROMLimits();
  void flex();
  void extend();
  ElbowBrace(int enc_sda, int enc_scl, int enc_dir, HexoBT* hexobt, BraceSettings* settings_ptr = NULL);
};

#endif