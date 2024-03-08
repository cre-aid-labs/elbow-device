#ifndef AUTH_H
#define AUTH_H

#include "hexobt.h"
#include "saved_data.h"

class HexoAuth {
  HexoBT* hexobt;
  LoginData login_data;
  void computeSHA1(const char* data, size_t dataLength, unsigned char* outputBuffer);
  String bytesToHexString(const unsigned char* bytes, size_t length);
  public:
  HexoAuth(HexoBT* hexobt = NULL);
  bool verifyPassword(char* hash);
  bool verifyPIN(char* hash);
  void modifyPIN(char* hash);
  void modifyPassword(char* hash);
}

#endif