#ifndef SAVED_DATA_H
#define SAVED_DATA_H

#include <Preferences.h>

class Login {
  Preferences login_data;
  char digest[32];

  public:
  void init();
  bool checkLogin(const char* password);
  bool setLogin(const char* password);
  ~Login();
};

class BraceSettings {
  Preferences brace_data;

  public:
  void init();

  float getOffsetAngle();
  bool setOffsetAngle(float angle);

  void setFlexionLimit(float angle);
  void setExtensionLimit(float angle);
  float getFlexionLimit();
  float getExtensionLimit();

  void setSpeed(int speed);
  int getSpeed();

  ~BraceSettings();
};



#endif