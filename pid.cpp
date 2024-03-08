#include "pid.h"
#include <Arduino.h>

#define PID_DELTA_THRESHOLD 1000
#define PID_TIME_DECEL_FACT 1000.0
#define PID_INTEGRAL_LIMIT 200.0
#define PID_MAX_POWER 200.0

PIDController::PIDController(float Kp, float Ki, float Kd, float init_sp) {
  this -> Kp = Kp;
  this -> Ki = Ki;
  this -> Kd = Kd;
  integral = 0.0;
  sp = init_sp;
  prev_time = millis();
  pid_max_power = PID_MAX_POWER;
}

void PIDController::setPoint(float sp) {
  this -> sp = sp;
}

float PIDController::out() {
  return u;
}

float PIDController::getIntegralValue() {
  return integral;
}

void PIDController::unwindIntegral() {
  integral = 0.0;
}

void PIDController::update(float cur_u) {
  unsigned long cur_time = millis();
  unsigned long delta = cur_time - prev_time;
  if(delta > PID_DELTA_THRESHOLD) {
    prev_time = cur_time;
    return;
  }
  prev_time = cur_time;
  float cur_e = cur_u - sp;
  float dt = (float) delta / 1000.0;
  float de = cur_e - prev_e;
  float dedt = de / dt;
  integral += cur_e * dt;
  if(Ki*integral > PID_INTEGRAL_LIMIT) integral = PID_INTEGRAL_LIMIT / Ki;
  if(Ki*integral < -PID_INTEGRAL_LIMIT) integral = -PID_INTEGRAL_LIMIT / Ki;
  u = -Kp * cur_e - Ki * integral - Kd * dedt;
  if(u > pid_max_power) u = pid_max_power;
  if(u < -pid_max_power) u = -pid_max_power;
  prev_e = cur_e;
}