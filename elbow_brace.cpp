#include "elbow_brace.h"
#include "AS5600.h"
#include "hexobt.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

ElbowBrace::ElbowBrace(int enc_sda, int enc_scl, int enc_dir, HexoBT* hexobt) {
  this -> enc_sda = enc_sda;
  this -> enc_scl = enc_scl;
  this -> enc_dir = enc_dir;
  this -> hexobt = hexobt;
  Wire.setPins(enc_sda, enc_scl);
  elbow_encoder = new AS5600();
  Wire.begin();
}

void ElbowBrace::controlLoop() {
  while(true){
    theta = elbow_encoder -> readAngle() * AS5600_RAW_TO_DEGREES;
    if(angle_control && theta > 0.0) {
      float delta = theta - set_point;
      if (abs(theta) < ANGLE_CTRL_THRESHOLD) {
        if (delta < 0) {
          controller -> forward();
        }
        else {
          controller -> backward();
        }
      }
    }
    vTaskDelay(300/portTICK_PERIOD_MS);
  }
}

void ElbowBrace::serialTransmitLoop() {
  while(true) {
    hexobt -> write("A ");
    hexobt -> write(std::to_string(this -> getAngle()));
    hexobt -> write("\n");
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

void ElbowBrace::serialTransmitLoop() {
  while(true) {
    hexobt -> write("A ");
    hexobt -> write(std::to_string(this -> getAngle()));
    hexobt -> write("\n");
    vTaskDelay(500/portTICK_PERIOD_MS);
  }
}

void ElbowBrace::setReference() {
  elbow_encoder -> setOffset(-theta);
}

float ElbowBrace::getAngle() {
  return theta;
}

void ElbowBrace::setAngle(float angle) {
  angle_control = true;
  set_point = angle;
}

void ElbowBrace::moveByAngle(float angle) {
  set_point += angle;
}

bool ElbowBrace::isAngleControlEnabled() {
  return angle_control;
}

void ElbowBrace::disableAngleControl() {
  angle_control = false;
}

void ElbowBrace::enableAngleControl() {
  angle_control = true;
}

void ElbowBrace::enableROMLimits() {
  rom_limit_enabled = true;
}

void ElbowBrace::setFlexLimitAtPosition() {
  flex_rom_limit = theta;
}

void ElbowBrace::setExtLimitAtPosition() {
  ext_rom_limit = theta;
}

void ElbowBrace::flex() {
  set_point = flex_rom_limit;
}

void ElbowBrace::extend() {
  set_point = ext_rom_limit;
}

void ElbowBrace::setController(LAController* controller) {
  this -> controller = controller;
}

void ElbowBrace::controlLoopWrapper(void* obj) {
  ElbowBrace* brace = (ElbowBrace*) obj;
  brace -> controlLoop();
}

void ElbowBrace::serialTransmitLoopWrapper(void* obj) {
  ElbowBrace* brace = (ElbowBrace*) obj;
  brace -> serialTransmitLoop();
}

void ElbowBrace::initDevice() {
  elbow_encoder -> begin(enc_dir);
  elbow_encoder -> setDirection(AS5600_CLOCK_WISE);
  if(!(elbow_encoder -> isConnected())) {
    Serial.println("Error! Elbow encoder is not connected.");
    return;
  }
  xTaskCreate(
    ElbowBrace::controlLoopWrapper,
    "Elbow Encoder",
    2000,
    (void*) this,
    1,
    &elbow_task
  );
  xTaskCreate(
    ElbowBrace::serialTransmitLoopWrapper,
    "Elbow Bluetooth Transmit",
    2000,
    (void*) this,
    1,
    &elbow_serial_task
  );
  xTaskCreate(
    ElbowBrace::serialTransmitLoopWrapper,
    "Elbow Bluetooth Transmit",
    2000,
    (void*) this,
    1,
    elbow_serial_task
  );
}