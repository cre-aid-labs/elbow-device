#ifndef SAVED_DATA_H
#define SAVED_DATA_H

#include <Preferences.h>

class LoginData {
  Preferences login_data;
  public:
  void init();
  String returnPasswordHash();
  String returnPasswordSalt();
  void setPasswordHash(const unsigned char* password_hash);
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