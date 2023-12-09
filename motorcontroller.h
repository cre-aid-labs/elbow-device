#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

struct LAControlState {
  float pos;
  float vel;
  float rot;
};

class LAControl {
  public:
  static const int 
    STOP = 0,
    FWD = 1,
    REV = 2;
};

class LAController {
  TaskHandle_t la_task;

  int lpwm_pin;
  int rpwm_pin;
  int min_lim_pin;
  int max_lim_pin;
  int enc_a_pin;
  int enc_b_pin;

  int mot_spd;
  int mot_mode;
  
  int min_pos;
  int max_pos;

  float omega;

  long enc_pulses;
  long prev_enc_pulses;
  unsigned long prev_enc_micros;
  unsigned long cur_enc_micros;

  bool has_limit_switch;
  bool has_encoder;

  bool is_control_status_changed;

  void controlLoop();

  public:

  LAController(int lpwm_pin, int rpwm_pin);

  void attachLimitSwitches(int min_lim_pin, int max_lim_pin);
  void attachEncoder(int enc_a, int enc_b);

  void initController();

  void homeControls();
  void fullForward();
  void fullBackward();
  void forward();
  void backward();
  void stop();

  void encoderISR();

  void set(int spd, int ctrl);
  void setMode(int ctrl);
  void setSpeed(int spd);

  int getMotorMode();
  float getRotations();
  float getRotSpeed();

  int isLimitSwitchTriggered();

  static void controlLoopWrapper(void* obj);
};

class PIDController {
  float Ki;
  float Kp;
  float Kd;
  unsigned long prev_time;
  float sp_err;
  float sp;
  float u;
  float integral;
  float prev_e;
  public:
  
  PIDController(float Ki, float Kp, float Kd);
  void setPoint(float sp);
  void update(float cur_u);
  float out();
  
};

#endif
