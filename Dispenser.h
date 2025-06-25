#ifndef DISPENSER_H
#define DISPENSER_H

#include <ESP32Servo.h>

extern Servo myServo;
extern const int SERVO_PIN;
extern String Dispenser_state;

void initDispenser();

#endif
