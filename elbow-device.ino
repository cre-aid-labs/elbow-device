#define MOTOR_VCC 4
#define MOTOR_VEE 13
#define MOTOR_CUR 0

#define K_LPF 0.3
#define CUR_SEN_ZERO 3740
#define CUR_SEN_SCALE 0.002
#define CUR_THRESHOLD 0.05

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

#include "motorcontroller.h"
#include "hexobt.h"
#include "status_led.h"
#include "parser.h"

void executeCommand(Command comm);
void emergencyStop();
void initMotorControlLoop();
void commExec(Command comm);

LAController* controller = NULL;
StatusLED* led_s = NULL;
HexoBT* hexobt;
Parser* parser;
BLEUUIDs* uuids;
TaskHandle_t la_task;
float current = 0;
float prev_current = 0;

bool is_command_being_executed = false;
Command cur_comm;

class ElbowDeviceServerCallbacks : public HexoBTServerCallbacks {
  void onConnect(BLEServer* pServer) {
    hexobt -> device_connected = true;
    led_s -> setStatus(STATUS_GOOD);
  }
  void onDisconnect(BLEServer* pServer) {
    hexobt -> device_connected = false;
    led_s -> setStatus(STATUS_NOCO);
  }
};

class ElbowDeviceRWCallbacks : public HexoBTCharacteristicCallbacks {
  void onWrite(BLECharacteristic* pTxCharacteristic) {
    char* rx_string = &(pTxCharacteristic -> getValue())[0];
    Command comm = parser -> parseDirect(rx_string);
    Serial.print("command: ");
    Serial.print(comm.mode);
    Serial.print(", ");
    Serial.println(comm.value);
    if(comm.mode == 'S') {
      emergencyStop();
    }else {
      executeCommand(comm);
    }
  }
};

void setup() {

  Serial.begin(115200);

  led_s = new StatusLED(2);
  StatusLED::initLED(led_s);

  controller = new LAController(MOTOR_VCC, MOTOR_VEE);

  parser = new Parser();
  parser -> init();

  uuids = new BLEUUIDs(
    CHARACTERISTIC_UUID_TX,
    CHARACTERISTIC_UUID_RX,
    SERVICE_UUID);
  hexobt = new HexoBT();
  hexobt -> init(
    "CREAID CPM",
    new ElbowDeviceServerCallbacks(),
    new ElbowDeviceRWCallbacks(),
    uuids);
  initMotorControlLoop();
}

void loop() {

  if(Serial.available()>0){
    char cmd = Serial.read();
    switch(cmd) {
      case 'F':
        controller->fullForward();
        Serial.println("Forward");
        break;
      case 'B':
        controller->fullBackward();
        Serial.println("Reverse");
        break;
      case 'S':
        controller->stop();
        Serial.println("Stopped");
        break;
    }
  }

  if(!(hexobt -> device_connected) && (hexobt -> prev_device_connected)) {
    hexobt -> restartAdvertising();
    hexobt -> prev_device_connected = hexobt -> device_connected;
  }
  if((hexobt -> device_connected) && !(hexobt -> prev_device_connected)) {
    hexobt -> prev_device_connected = hexobt -> device_connected;
  }
}

float readCurrent() {
  float analog_current = (analogRead(MOTOR_CUR) - CUR_SEN_ZERO) * CUR_SEN_SCALE;
  current = prev_current + K_LPF * (analog_current - prev_current);
  prev_current = current;
  return abs(current);
}

void initMotorControlLoop() {
  xTaskCreate(
    commandLoop,
    "LA Command loop",
    10000,
    NULL,
    1,
    &la_task
  );
}

void emergencyStop() {
  is_command_being_executed = false;
  controller -> stop();
  vTaskDelete(la_task);
  Serial.println("task deleted");
  vTaskDelay(50/portTICK_PERIOD_MS);
  initMotorControlLoop();
}

void executeCommand(Command comm) {
  cur_comm = comm;
  is_command_being_executed = true;
}

void commandLoop(void* obj) {
  while(true) {
    if(is_command_being_executed) {
      commExec(cur_comm);
      is_command_being_executed = false;
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void commExec(Command comm) {
  switch(comm.mode) {
    case 'E':
      if(comm.value > 100){
        controller -> fullForward();
      }else {
        controller -> forward(comm.value);
      }
      break;
    case 'F':
      if(comm.value > 100){
        controller -> fullBackward();
      }else {
        controller -> backward(comm.value);
      }
      break;
    case 'C':
      for(int i=0; i<comm.value; i++) {
        controller -> fullForward();
        vTaskDelay(4600/portTICK_PERIOD_MS);
        controller -> fullBackward();
        vTaskDelay(4600/portTICK_PERIOD_MS);
      }
      break;
    default:
      controller -> stop();
  }
}
