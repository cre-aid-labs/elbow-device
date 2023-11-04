#define MOTOR_VCC 4
#define MOTOR_VEE 13
#define MOTOR_CUR 0

#define K_LPF 0.3
#define CUR_SEN_ZERO 3740
#define CUR_SEN_SCALE 0.002
#define CUR_THRESHOLD 0.05

//encoder
#define ENC_A 6
#define ENC_B 7

// limit switches
#define LIM_SMIN 0
#define LIM_SMAX 1

// motor pins
#define MOT_1 4
#define MOT_2 5

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

void IRAM_ATTR encoderISR_A();

int s_mot = 0;

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

  controller = new LAController(MOT_1, MOT_2);
  controller -> attachLimitSwitches(LIM_SMIN, LIM_SMAX);
  controller -> attachEncoder(ENC_A, ENC_B);
  attachInterrupt(ENC_A, encoderISR_A, RISING);
  controller -> initController();
  controller -> set(0, LAControl::STOP);

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
        controller -> set(s_mot, LAControl::FWD);
        break;
      case 'B':
        controller -> set(s_mot, LAControl::REV);
        break;
      case 'S':
        controller -> set(0, LAControl::STOP);
        break; 
      case 'H':
        controller -> homeControls();
        break;
      case 'E':
        s_mot = Serial.readStringUntil('/').toInt();
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
  controller -> set(0, LAControl::STOP);
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
  int cpm_counts;
  switch(comm.mode) {
    case 'E':
      if(comm.value > 200){
        controller -> set(200, LAControl::REV);
      }else {
        controller -> set(comm.value, LAControl::REV);
      }
      break;
    case 'F':
      if(comm.value > 200){
        controller -> set(200, LAControl::FWD);
      }else {
        controller -> set(comm.value, LAControl::FWD);
      }
      break;
    case 'C':
      cpm_counts = 0;
      while(cpm_counts < comm.value) {
        if(controller -> isLimitSwitchTriggered() > 0) {
          controller -> set(100, LAControl::REV);
        } else if(controller -> isLimitSwitchTriggered() < 0) {
          controller -> set(100, LAControl::FWD);
          cpm_counts++;
        }
        vTaskDelay(200/portTICK_PERIOD_MS);
      }
      break;
    default:
      controller -> stop();
  }
}

void IRAM_ATTR encoderISR_A() {
  controller -> encoderISR();
}