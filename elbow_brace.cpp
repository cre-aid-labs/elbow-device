#include "elbow_brace.h"
#include "AS5600.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

ElbowBrace::ElbowBrace(int enc_sda, int enc_scl, int enc_dir) {
  this -> enc_sda = enc_sda;
  this -> enc_scl = enc_scl;
  this -> enc_dir = enc_dir;
  elbow_encoder = new AS5600();
}

void ElbowBrace::controlLoop() {
  while(true){
    theta = elbow_encoder -> readAngle() * AS5600_RAW_TO_DEGREES;
    vTaskDelay(300/portTICK_PERIOD_MS);
  }
}

void ElbowBrace::setReference() {
  elbow_encoder -> setOffset(theta);
}

float ElbowBrace::getAngle() {
  return theta;
}

void ElbowBrace::controlLoopWrapper(void* obj) {
  ElbowBrace* brace = (ElbowBrace*) obj;
  brace -> controlLoop();
}

void ElbowBrace::initDevice() {
  elbow_encoder -> begin(enc_sda, enc_scl, enc_dir);
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
}