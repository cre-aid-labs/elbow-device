#include "saved_data.h"
#include <Preferences.h>

void Login::init() {
  login_data.begin("login-data");
}

bool Login::checkLogin(const char *password) {
  if (strlen(password) != 4) return false;
  return login_data.getString("pin", "0000").equals(password);
}

bool Login::setLogin(const char *password) {
  if (strlen(password) != 4) return false;
  login_data.putString("pin", password);
  return true;
}

Login::~Login() {
  login_data.end();
}

void BraceSettings::init() {
  brace_data.begin("brace-data");
}

float BraceSettings::getOffsetAngle() {
  return brace_data.getFloat("encoder_offset", 0.0);
}

bool BraceSettings::setOffsetAngle(float angle) {
  if(angle > 360.0) return false;
  brace_data.putFloat("encoder_offset", angle);
  return true;
}

void BraceSettings::setFlexionLimit(float angle) {
  if(angle > 120.0) angle = 120.0;
  brace_data.putFloat("flex_limit", angle);
}

void BraceSettings::setExtensionLimit(float angle) {
  if(angle < 0.0) angle = 0.0;
  brace_data.putFloat("ext_limit", angle);
}

float BraceSettings::getExtensionLimit() {
  return brace_data.getFloat("ext_limit", 0.0);
}

float BraceSettings::getFlexionLimit() {
  return brace_data.getFloat("flex_limit", 120.0);
}

BraceSettings::~BraceSettings() {
  brace_data.end();
}