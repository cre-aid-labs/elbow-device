#ifndef PID_H
#define PID_H

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