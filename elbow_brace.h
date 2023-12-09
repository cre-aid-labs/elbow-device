#ifndef ELBOW_BRACE_H
#define ELBOW_BRACE_H
#include "motorcontroller.h"
#include "AS5600.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class ElbowBrace {
  AS5600* elbow_encoder = NULL;
  TaskHandle_t elbow_task;
  int enc_sda;
  int enc_scl;
  int enc_dir;

  float omega;
  float theta;

  void controlLoop();
  public:
  static void controlLoopWrapper(void* obj);
  void initDevice();
  void setReference();
  float getAngle();
  ElbowBrace(int enc_sda, int enc_scl, int enc_dir);
};

#endif