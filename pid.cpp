#include "pid.h"
#include <Arduino.h>

#define PID_DELTA_THRESHOLD 10000
#define PID_TIME_DECEL_FACT 1000.0

PIDController::PIDController(float Kp, float Ki, float Kd) {
  this -> Kp = Kp;
  this -> Ki = Ki;
  this -> Kd = Kd;
}

void PIDController::setPoint(float sp) {
  this -> sp = sp;
}

float PIDController::out() {
  return u;
}

void PIDController::update(float cur_u) {
  unsigned long cur_time = micros();
  unsigned long delta = cur_time - prev_time;
  if(delta > PID_DELTA_THRESHOLD) {
    prev_time = cur_time;
    return;
  }
  prev_time = cur_time;
  float dt = (float) delta / 1000000.0;

  float cur_e = cur_u - sp;
  float dedt = (cur_e - prev_e) / dt;
  integral += (cur_e - prev_e) * dt;
  u = Kp * cur_e + Ki * integral + Kd * dedt;
  prev_e = cur_e;
}