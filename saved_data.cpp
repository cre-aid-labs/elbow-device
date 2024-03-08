#include "saved_data.h"
#include <Preferences.h>
#include <crypto.h>

void LoginData::init() {
  login_data.begin("login-data");
}

void LoginData::setPasswordHash(String password_hash, String password_salt) {
  login_data.putString("sha1-hash", password_hash);
  login_data.putString("pwd-salt", password_salt);
}

String LoginData::returnPasswordHash(const unsigned char* password_hash_buffer) {
  return login_data.getString("sha1-hash", "00000000000000000000");
}

String LoginData::returnPasswordSalt() {
  return login_data.getString("pwd-salt", "0000000000");
}

LoginData::~LoginData() {
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