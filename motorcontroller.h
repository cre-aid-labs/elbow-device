#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

struct LAControlPinBundle {
  int lpwm_pin;
  int rpwm_pin;
  int min_lim_pin;
  int max_lim_pin;
};

struct LAControlState {
  float pos;
  float vel;
  float rot;
};

class LAController {
  int lpwm_pin;
  int rpwm_pin;
  int min_lim_pin;
  int max_lim_pin;
  bool has_limit_switch;
  int limit_sw_type;
  public:
  LAControlPinBundle pins;
  int min_pos;
  int max_pos;
  LAController(int lpwm_pin, int rpwm_pin);
  LAController(int lpwm_pin, int rpwm_pin, int min_lim_pin, int max_lim_pin);
  void defineLimitSwitches(int min_lim_pin, int max_lim_pin, bool pullup);
  void homeControls();
  void fullForward();
  void fullBackward();
  void forward(int spd);
  void backward(int spd);
  void stop();
};

#endif
