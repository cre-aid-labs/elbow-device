#ifndef PIN_DEFS_H
#define PIN_DEFS_H

#define MOTOR_VCC 4
#define MOTOR_VEE 13
#define MOTOR_CUR 0

#define K_LPF 0.3
#define CUR_SEN_ZERO 3740
#define CUR_SEN_SCALE 0.002
#define CUR_THRESHOLD 0.05

// encoder
#define ENC_A 6
#define ENC_B 7

// limit switches
#ifdef ESP32_C3
#define LIM_SMIN 0
#define LIM_SMAX 1
#endif
#ifdef ESP32_S3
#define LIM_SMIN 8
#define LIM_SMAX 9
#define LED_PIN 21
#endif

// motor pins
#define MOT_1 4
#define MOT_2 5

// led pin
#ifdef ESP32_C3
#define LED_PIN 2
#endif
#ifdef ESP32_S3
#define LED_PIN 21
#endif

// brace encoder pins
#ifdef BRACE_ENCODER
#define BRC_ENC_SDA 17
#define BRC_ENC_SCL 18
#define BRC_ENC_DIR 33
#endif

#endif