#include "motorcontroller.h"
#include <arduino.h>

#define LOOP_REFRESH_TP 100
#define LA_TASK_STACK_SIZE 3000
#define PULSES_PER_ROT 134.4

#define PID_DELTA_THRESHOLD 10000
#define PID_TIME_DECEL_FACT 1000.0

#define LACTRL_DELTA_THRESHOLD 10000000


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

void LAController::controlLoop() {
  while(true) {

    if(has_limit_switch) {
      if(isLimitSwitchTriggered() > 0 && mot_mode == LAControl::FWD) {
        mot_mode = LAControl::STOP;
      } else if(isLimitSwitchTriggered() < 0 && mot_mode == LAControl::REV) {
        mot_mode = LAControl::STOP;
      }
    }

    switch(mot_mode) {
      case LAControl::FWD:
        forward();
        break;
      case LAControl::REV:
        backward();
        break;
      case LAControl::STOP:
        stop();
        break;
      default:
        stop();
        break;
    }

    cur_enc_micros = micros();
    float delta_micros = cur_enc_micros - prev_enc_micros;
    if((LACTRL_DELTA_THRESHOLD > delta_micros) && (delta_micros > 0)) {
      omega = (enc_pulses - prev_enc_pulses) / delta_micros * 1000000.0;
      prev_enc_pulses = enc_pulses;
      prev_enc_micros = cur_enc_micros;
    } else {
      prev_enc_micros = cur_enc_micros;
    }
    vTaskDelay(LOOP_REFRESH_TP / portTICK_PERIOD_MS);
  }
}

void LAController::controlLoopWrapper(void* obj) {
  LAController* motor_obj = (LAController*) obj;
  motor_obj -> controlLoop();
}

LAController::LAController(int lpwm_pin, int rpwm_pin) {
  this -> lpwm_pin = lpwm_pin;
  this -> rpwm_pin = rpwm_pin;
  pinMode(lpwm_pin, OUTPUT);
  pinMode(rpwm_pin, OUTPUT);
  has_limit_switch = false;
  has_encoder = false;
  mot_mode = LAControl::STOP;
}

void LAController::attachLimitSwitches(int min_lim_pin, int max_lim_pin){
  this -> min_lim_pin = min_lim_pin;
  this -> max_lim_pin = max_lim_pin;
  pinMode(min_lim_pin, INPUT_PULLUP);
  pinMode(max_lim_pin, INPUT_PULLUP);
  has_limit_switch = true;
}

void LAController::attachEncoder(int enc_a, int enc_b) {
  enc_a_pin = enc_a;
  enc_b_pin = enc_b;
  pinMode(enc_a_pin, INPUT);
  pinMode(enc_b_pin, INPUT);
  enc_pulses = 0;
  has_encoder = true;
}

void LAController::encoderISR() {
  if(digitalRead(enc_b_pin) == HIGH) {
    enc_pulses++;
  } else {
    enc_pulses--;
  }
}

float LAController::getRotations() {
  return enc_pulses / PULSES_PER_ROT;
}

float LAController::getRotSpeed() {
  return omega / PULSES_PER_ROT;
}

void LAController::homeControls() {
  if(!(has_limit_switch && has_encoder)) return;
  if(isLimitSwitchTriggered() > 0) {
    set(30, LAControl::REV);
    while(isLimitSwitchTriggered() > 0) {
      vTaskDelay(50/portTICK_PERIOD_MS);
    }
    enc_pulses = 0;
    set(30, LAControl::REV);
    while(abs(enc_pulses) < 300) {
      vTaskDelay(50/portTICK_PERIOD_MS);
    }
  }
  set(30, LAControl::FWD);
  while(isLimitSwitchTriggered() == 0) {
    vTaskDelay(50/portTICK_PERIOD_MS);
  }
  enc_pulses = 0;
  set(0, LAControl::STOP);
}

void LAController::fullForward() {
  analogWrite(lpwm_pin, 255);
  analogWrite(rpwm_pin, 0);
}

void LAController::fullBackward() {
  analogWrite(lpwm_pin, 0);
  analogWrite(rpwm_pin, 255);
}

void LAController::forward() {
  analogWrite(lpwm_pin, mot_spd);
  analogWrite(rpwm_pin, 0);
}

void LAController::backward() {
  analogWrite(lpwm_pin, 0);
  analogWrite(rpwm_pin, mot_spd);
}

void LAController::stop() {
  analogWrite(lpwm_pin, 0);
  analogWrite(rpwm_pin, 0);
}

int LAController::isLimitSwitchTriggered() {
  if(digitalRead(min_lim_pin)) {
    return -1;
  }
  if(digitalRead(max_lim_pin)) {
    return 1;
  }
  return 0;
}

void LAController::set(int spd, int ctrl) {
  setMode(ctrl);
  setSpeed(spd);
}

void LAController::setMode(int ctrl) {
  switch(ctrl) {
    case LAControl::FWD:
      if(isLimitSwitchTriggered() > 0) break;
      mot_mode = LAControl::FWD;
      break;
    case LAControl::REV:
      if(isLimitSwitchTriggered() < 0) break;
      mot_mode = LAControl::REV;
      break;
    case LAControl::STOP:
      mot_mode = LAControl::STOP;
      break;
  }
}

void LAController::setSpeed(int spd) {
  mot_spd = spd;
}

int LAController::getMotorMode() {
  return mot_mode;
}

void LAController::initController() {
  xTaskCreate(
    LAController::controlLoopWrapper,
    "LACT",
    LA_TASK_STACK_SIZE,
    (void*) this,
    1,
    &la_task
  );
}