#include "auth.h"

#include <Preferences.h>
#include "mbedtls/md.h"

HexoAuth::HexoAuth(HexoBT* hexobt) {
  this -> hexobt = hexobt;
}

bool HexoAuth::verifyPassword(char* pin) {
  unsigned char hash[20];
  char salt[11];

  login_data.returnPasswordSalt(salt);

  //Salting format: <10-Char-Salt><6-Digit-PIN>
  char combined_pass[18];
  strcpy(combined_pass, salt);
  strcat(combined_pass, pin);

  computeSHA1(combined_pass, strlen(combined_pass), hash);
  String computed_hash_string = bytesToHexString(hash, 20);
  String stored_hash_string = login_data.returnPasswordHash();

  if(computed_hash_string.equals(stored_hash_string)) {
    return true;
  } else {
    return false;
  }
}

void HexoAuth::computeSHA1(const char* data, size_t dataLength, unsigned char* outputBuffer) {
  // Initialize mbedTLS library
  mbedtls_md_context_t ctx;
  mbedtls_md_init(&ctx);
  
  // Select SHA-1 algorithm
  const mbedtls_md_info_t *info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
  mbedtls_md_setup(&ctx, info, 0);
  
  // Compute SHA-1 hash
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char *)data, dataLength);
  mbedtls_md_finish(&ctx, outputBuffer);
  
  // Free resources
  mbedtls_md_free(&ctx);
}

String HexoAuth::bytesToHexString(const unsigned char* bytes, size_t length) {
  String hexString = "";
  for (size_t i = 0; i < length; i++) {
    char hexBuffer[3];
    snprintf(hexBuffer, sizeof(hexBuffer), "%02X", bytes[i]);
    hexString += hexBuffer;
  }
  return hexString;
}