#include "Dispenser.h"

Servo myServo;
const int SERVO_PIN = 26;
String Dispenser_state = "closed";

void initDispenser() {
  myServo.attach(SERVO_PIN);
  myServo.write(90);  // cerrado por defecto
}
