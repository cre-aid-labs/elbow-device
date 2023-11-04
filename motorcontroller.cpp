#include "motorcontroller.h"
#include <arduino.h>

LAController::LAController(int lpwm_pin, int rpwm_pin) {
  this -> lpwm_pin = lpwm_pin;
  this -> rpwm_pin = rpwm_pin;
  pinMode(lpwm_pin, OUTPUT);
  pinMode(rpwm_pin, OUTPUT);
  has_limit_switch = false;
}

void LAController::defineLimitSwitches(int min_lim_pin, int max_lim_pin, bool pullup=true){
  this -> min_lim_pin = min_lim_pin;
  this -> max_lim_pin = max_lim_pin;
  pinMode(min_lim_pin, pullup?INPUT_PULLUP:INPUT);
  pinMode(max_lim_pin, pullup?INPUT_PULLUP:INPUT);
  has_limit_switch = true;
}

void LAController::homeControls() {
  ;
}

void LAController::fullForward() {
  digitalWrite(lpwm_pin, HIGH);
  digitalWrite(rpwm_pin, LOW);
}

void LAController::fullBackward() {
  digitalWrite(lpwm_pin, LOW);
  digitalWrite(rpwm_pin, HIGH);
}

void LAController::forward(int spd) {
  analogWrite(lpwm_pin, spd);
  analogWrite(rpwm_pin, 0);
}

void LAController::backward(int spd) {
  analogWrite(lpwm_pin, 0);
  analogWrite(rpwm_pin, spd);
}

void LAController::stop() {
  digitalWrite(lpwm_pin, LOW);
  digitalWrite(rpwm_pin, LOW);
}
