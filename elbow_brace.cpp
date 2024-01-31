#include <string>
#include "elbow_brace.h"
#include "AS5600.h"
#include "hexobt.h"
#include "Wire.h"
#include "saved_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define BRACE_PID_P 16.0
#define BRACE_PID_I 5.0
#define BRACE_PID_D 0.0

ElbowBrace::ElbowBrace(int enc_sda, int enc_scl, int enc_dir, HexoBT* hexobt, BraceSettings* settings_ptr) {
  this -> enc_sda = enc_sda;
  this -> enc_scl = enc_scl;
  this -> enc_dir = enc_dir;
  this -> hexobt = hexobt;
  Wire.setPins(enc_sda, enc_scl);
  elbow_encoder = new AS5600();
  Wire.begin();

  pid = new PIDController(BRACE_PID_P, BRACE_PID_I, BRACE_PID_D);

  if(settings_ptr == NULL) {
    settings = new BraceSettings();
  } else {
    settings = settings_ptr;
  }

  elbow_encoder -> setOffset(settings -> getOffsetAngle());
  theta = elbow_encoder -> readAngle() * AS5600_RAW_TO_DEGREES;
  if(theta > 180.0) theta -= 360.0;
  if(theta < -180.0) theta += 360.0;
  
  set_point = theta;
  pid -> setPoint(set_point);
  pid -> update(theta);
  
}

void ElbowBrace::controlLoop() {
  while(true){
    theta = elbow_encoder -> readAngle() * AS5600_RAW_TO_DEGREES;
    if(theta > 180.0) theta -= 360.0;
    if(theta < -180.0) theta += 360.0;
    /*
    if(angle_control && theta >= 0.0) {
      float delta = theta - set_point;
      if (abs(delta) > ANGLE_CTRL_THRESHOLD) {
        if (delta < 0) {
          controller -> set(100, LAControl::FWD);
        }
        else {
          controller -> set(100, LAControl::REV);
        }
      } else {
        controller -> set(0, LAControl::STOP);
      }
    }
    */

    if(angle_control) {
      float delta = theta - set_point;
      if(abs(delta) > ANGLE_CTRL_THRESHOLD) {
        float control_sig = pid -> out();
        if(control_sig > 0) {
          controller -> set(control_sig, LAControl::FWD);
        } else {
          controller -> set(-control_sig, LAControl::REV);
        }
      } else {
        controller -> set(0, LAControl::STOP);
      }
    }

    if(rom_limit_enabled && !angle_control) {
      if(theta > flex_rom_limit || theta < ext_rom_limit) {
        if(controller -> isMoving()) {
          switch(controller -> getMotorMode()) {
            case LAControl::FWD:
              if(theta > flex_rom_limit) controller -> set(0, LAControl::STOP);
              break;
            case LAControl::REV:
              if(theta < ext_rom_limit) controller -> set(0, LAControl::STOP);
              break;
          }
        }
      }
    }
    pid -> update(theta);
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void ElbowBrace::serialTransmitLoop() {
  while(true) {
    hexobt -> write("A " + std::to_string(this -> getAngle()));
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
  float cur_angle = elbow_encoder -> readAngle() * AS5600_RAW_TO_DEGREES;
  float cur_offset = elbow_encoder -> getOffset();
  elbow_encoder -> setOffset(360.0 - cur_angle + cur_offset);
}

float ElbowBrace::getReference() {
  return elbow_encoder -> getOffset();
}

float ElbowBrace::getAngle() {
  return theta;
}

void ElbowBrace::setAngle(float angle) {
  set_point = angle;
  pid -> setPoint(set_point);
  pid -> unwindIntegral();
}

void ElbowBrace::moveByAngle(float angle) {
  set_point += angle;
  pid -> setPoint(set_point);
}

bool ElbowBrace::isAngleControlEnabled() {
  return angle_control;
}

void ElbowBrace::disableAngleControl() {
  angle_control = false;
}

void ElbowBrace::enableAngleControl() {
  setAngle(getAngle());
  pid -> unwindIntegral();
  angle_control = true;
}

void ElbowBrace::enableROMLimits() {
  rom_limit_enabled = true;
}

void ElbowBrace::disableROMLimits() {
  rom_limit_enabled = false;
}

void ElbowBrace::setFlexLimit(float angle, bool save_to_device) {
  flex_rom_limit = angle;
  if(save_to_device) {
    settings -> setFlexionLimit(angle);
  }
}

void ElbowBrace::setExtLimit(float angle, bool save_to_device) {
  ext_rom_limit = angle;
  if(save_to_device) {
    settings -> setFlexionLimit(angle);
  }
}

void ElbowBrace::setFlexLimitAtPosition(bool save_to_device) {
  this -> setFlexLimit(theta, save_to_device);
}

void ElbowBrace::setExtLimitAtPosition(bool save_to_device) {
  this -> setExtLimit(theta, save_to_device);
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

bool ElbowBrace::isROMLimitEnabled() {
  return rom_limit_enabled;
}

void ElbowBrace::getPreferences() {
  if(hexobt == NULL) return;
  hexobt -> write("P0 " + std::to_string(isAngleControlEnabled() ? 1:0));
  hexobt -> write("P1 " + std::to_string(isROMLimitEnabled() ? 1:0));
  hexobt -> write("P2 " + std::to_string(flex_rom_limit));
  hexobt -> write("P3 " + std::to_string(ext_rom_limit));
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
    3000,
    (void*) this,
    1,
    elbow_task
  );
  xTaskCreate(
    ElbowBrace::serialTransmitLoopWrapper,
    "Elbow Bluetooth Transmit",
    3000,
    (void*) this,
    1,
    elbow_serial_task
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